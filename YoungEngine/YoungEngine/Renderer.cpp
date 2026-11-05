#include "Renderer.hpp"

Renderer::Renderer(
	Window &inWindow,
	Scene &inScene,
	size_t framesInFlight
) : window(inWindow),
	scene(&inScene),
	sync(framesInFlight),
	command(framesInFlight) {

	frameUBOs.reserve(framesInFlight);
	frameSets.reserve(framesInFlight);

	for (size_t i = 0; i < framesInFlight; i++) {
		frameUBOs.push_back(std::make_unique<UniformBufferObject>(
			sizeof(Camera::UniformBlock),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		));
		frameUBOs[i]->createBuffer();

		frameSets.push_back(std::make_unique<DescriptorSet>(PipelineDescription::frameBindings()));
		frameSets[i]->writeUniformBuffer(PipelineDescription::FRAME_CAMERA_BINDING, *frameUBOs[i]);
	}
}

void Renderer::render(bool externalResizeRequested) {

	sync.stop(currentFrame);

	updateFrame(currentFrame);

	uint32_t imageIndex;
	SwapChain::AcquireResult acquireResult = SwapChain::active->acquireNextImage(
		sync.imageAvailable(currentFrame),
		imageIndex
	);
	if (acquireResult == SwapChain::AcquireResult::OutOfDate) {
		recreateSwapChain();
		return;
	}

	sync.go(currentFrame);

	VkCommandBuffer commandBuffer = command.begin(currentFrame);
	recordFrame(commandBuffer, imageIndex, currentFrame);
	command.end(currentFrame);

	submitFrame(
		currentFrame,
		sync.imageAvailable(currentFrame),
		sync.renderFinished(currentFrame),
		sync.inFlightFence(currentFrame)
	);

	bool needsRecreate = SwapChain::active->present(sync.renderFinished(currentFrame), imageIndex);
	if (needsRecreate || externalResizeRequested) {
		recreateSwapChain();
	}

	currentFrame = (currentFrame + 1) % sync.count();
}

void Renderer::updateFrame(size_t frame) {

	Camera &camera = scene->getCamera();
	camera.setViewportExtent(SwapChain::active->swapChainExtent.width, SwapChain::active->swapChainExtent.height);

	Camera::UniformBlock cameraBlock = camera.getUniformBlock();
	frameUBOs[frame]->updateBuffer(&cameraBlock, sizeof(cameraBlock));

	PipelineDescription::RenderingFormats formats = renderTarget.renderingFormats();

	drawItems.clear();
	for (const auto &renderable : scene->getRenderables()) {
		renderable->prepare(formats, SwapChain::active->swapChainExtent);
		renderable->upload(frame);
		renderable->collect(frame, drawItems);
	}

	//The items of a material are put together, in the order of its passes. Within one pass of one material
	//they're sorted by pipeline and set, so that they're bound as few times as possible.
	std::sort(
		drawItems.begin(),
		drawItems.end(),
		[](const DrawItem &a, const DrawItem &b) {
			if (a.material != b.material) return std::less<const Material *>()(a.material, b.material);
			if (a.pass != b.pass) return a.pass < b.pass;
			if (a.pipeline != b.pipeline) return std::less<const Pipeline *>()(a.pipeline, b.pipeline);
			return std::less<VkDescriptorSet>()(a.materialSet, b.materialSet);
		}
	);

	//Every prop collects the fullscreen passes of its material, but each one is drawn only once per material.
	//After the sort the copies are next to each other, so all but the first are dropped.
	drawItems.erase(
		std::unique(
			drawItems.begin(),
			drawItems.end(),
			[](const DrawItem &a, const DrawItem &b) {
				return a.material && a.material == b.material && a.pass == b.pass
					&& a.material->getPass(a.pass).geometry == MaterialPass::Geometry::Fullscreen;
			}
		),
		drawItems.end()
	);

	//The items that render into textures go before the ones drawn into the screen, keeping the order above
	auto firstScreen = std::stable_partition(
		drawItems.begin(),
		drawItems.end(),
		[](const DrawItem &item) { return targetOf(item) != nullptr; }
	);
	firstScreenItem = static_cast<size_t>(firstScreen - drawItems.begin());
}

void Renderer::recordFrame(VkCommandBuffer commandBuffer, uint32_t imageIndex, size_t frame) {

	//The command buffer has already been begun. First the passes that render into textures, each in its own
	//rendering scope, so that the screen's passes after them can sample what they rendered. Then the parts
	//that happen once per frame for the screen: the transitions of its images and its rendering scope
	//(the attachments are cleared when it begins). Everything in between is recorded from the draw items.

	recordTexturePasses(commandBuffer, frame);

	//Without a render pass, nothing transitions the layouts of the attachments or synchronizes
	//access to them for us. That's why the rendering is bracketed by explicit image barriers.

	VkImage swapChainImage = SwapChain::active->swapChainImages[imageIndex];
	VkImageView swapChainImageView = SwapChain::active->swapChainImageViews[imageIndex];
	VkExtent2D extent = SwapChain::active->swapChainExtent;

	VkImageAspectFlags depthAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	if (ImageUtility::hasStencilComponent(renderTarget.depthFormat())) {
		depthAspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	//BARRIERS BEFORE RENDERING
	//All three images are written from scratch (cleared, or fully resolved into)

	//The swap chain image receives the resolved result
	ImageUtility::recordImageLayoutTransition(
		commandBuffer,
		swapChainImage,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	);

	//The multisampled color image and the depth image are shared between the frames in flight
	ImageUtility::recordImageLayoutTransition(
		commandBuffer,
		renderTarget.CIO.image,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	);

	ImageUtility::recordImageLayoutTransition(
		commandBuffer,
		renderTarget.DIO.image,
		depthAspect,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
	);

	//Describing the attachments directly, instead of through a render pass and a framebuffer.
	//The color attachment is the multisampled image, which is resolved into the swap chain image
	//at the end of rendering. The multisampled contents aren't needed afterwards, so they're not stored.
	VkClearValue colorClearValue{};
	colorClearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

	VkRenderingAttachmentInfo colorAttachment{};
	colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	colorAttachment.imageView = renderTarget.CIO.imageView;
	colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
	colorAttachment.resolveImageView = swapChainImageView;
	colorAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.clearValue = colorClearValue;

	VkClearValue depthClearValue{};
	depthClearValue.depthStencil = { 1.0f, 0 };

	VkRenderingAttachmentInfo depthAttachment{};
	depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	depthAttachment.imageView = renderTarget.DIO.imageView;
	depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depthAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.clearValue = depthClearValue;

	VkRenderingInfo renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderingInfo.renderArea.offset = { 0, 0 };
	renderingInfo.renderArea.extent = extent;
	renderingInfo.layerCount = 1;
	renderingInfo.viewMask = 0;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachment;
	renderingInfo.pDepthAttachment = &depthAttachment;
	renderingInfo.pStencilAttachment = nullptr;

	//Thus begins the recording of commands of the rendering
	vkCmdBeginRendering(commandBuffer, &renderingInfo);

	setViewportAndScissor(commandBuffer, extent);

	recordDrawItems(commandBuffer, frame, firstScreenItem, drawItems.size());

	vkCmdEndRendering(commandBuffer);

	//BARRIER AFTER RENDERING
	//The resolved swap chain image goes to the layout required for presentation.
	//The render pass used to do this through the finalLayout of the resolve attachment.
	ImageUtility::recordImageLayoutTransition(
		commandBuffer,
		swapChainImage,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0
	);
}

void Renderer::recordTexturePasses(VkCommandBuffer commandBuffer, size_t frame) {

	//The items before firstScreenItem render into textures. The items of one pass of one material are next to
	//each other and all render into the same texture, so each such run is one rendering scope. Since the passes
	//of a material are in order, a pass that samples the texture of an earlier pass is recorded after it.

	size_t first = 0;

	while (first < firstScreenItem) {

		const DrawItem &firstItem = drawItems[first];

		size_t last = first + 1;
		while (last < firstScreenItem
			&& drawItems[last].material == firstItem.material
			&& drawItems[last].pass == firstItem.pass) {
			last++;
		}

		const Texture &target = *firstItem.material->getTarget(firstItem.pass);

		target.recordBeginRendering(commandBuffer, firstItem.material->getTargetDepth(firstItem.pass));
		setViewportAndScissor(commandBuffer, target.getExtent());
		recordDrawItems(commandBuffer, frame, first, last);
		target.recordEndRendering(commandBuffer);

		first = last;
	}
}

void Renderer::recordDrawItems(VkCommandBuffer commandBuffer, size_t frame, size_t first, size_t last) {

	VkDescriptorSet frameSet = frameSets[frame]->getHandle();

	const Pipeline *boundPipeline = nullptr;
	VkDescriptorSet boundMaterialSet = VK_NULL_HANDLE;

	for (size_t i = first; i < last; i++) {

		const DrawItem &item = drawItems[i];

		VkPipelineLayout layout = item.pipeline->pipelineLayout;

		if (item.pipeline != boundPipeline) {
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, item.pipeline->graphicsPipeline);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, PipelineDescription::DescriptorTier::Frame, 1, &frameSet, 0, nullptr);
			boundPipeline = item.pipeline;

			//A pipeline with a differently shaped layout (other bindings, other push constants) disturbs the sets
			//bound before it, so the material set is bound again even if it's the same one.
			boundMaterialSet = VK_NULL_HANDLE;
		}

		if (item.materialSet != VK_NULL_HANDLE && item.materialSet != boundMaterialSet) {
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, PipelineDescription::DescriptorTier::Material, 1, &item.materialSet, 0, nullptr);
			boundMaterialSet = item.materialSet;
		}

		if (item.objectSet != VK_NULL_HANDLE) {
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, PipelineDescription::DescriptorTier::Object, 1, &item.objectSet, 0, nullptr);
		}

		if (!item.pushConstants.empty()) {
			vkCmdPushConstants(commandBuffer, layout, item.pushConstantStages, 0, static_cast<uint32_t>(item.pushConstants.size), item.pushConstants.data);
		}

		//A fullscreen pass has no vertex buffer, its vertex shader makes the vertices
		if (item.vertexBuffer != VK_NULL_HANDLE) {
			VkDeviceSize offset = 0;
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &item.vertexBuffer, &offset);
		}

		if (item.indexBuffer != VK_NULL_HANDLE) {
			vkCmdBindIndexBuffer(commandBuffer, item.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(commandBuffer, item.count, 1, 0, 0, 0);
		} else {
			vkCmdDraw(commandBuffer, item.count, 1, 0, 0);
		}
	}
}

void Renderer::submitFrame(size_t frame, VkSemaphore imageAvailable, VkSemaphore renderFinished, VkFence inFlight) {

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailable };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkCommandBuffer commandBuffer = command.getBuffer(frame);

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkSemaphore signalSemaphores[] = { renderFinished };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(Device::active->graphicsQueue, 1, &submitInfo, inFlight) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer!");
	}
}

void Renderer::recreateSwapChain() {

	int width = 0;
	int height = 0;
	glfwGetFramebufferSize(window.window, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window.window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(Device::active->logical);

	SwapChain::active->recreate(window.window);
	SwapChain::active->createImageViews();

	renderTarget.recreateAttachments();
}

const Texture *Renderer::targetOf(const DrawItem &item) {

	return item.material ? item.material->getTarget(item.pass) : nullptr;
}

void Renderer::setViewportAndScissor(VkCommandBuffer commandBuffer, VkExtent2D extent) {

	//Viewport and scissor are dynamic state of every pipeline, so they're set once per rendering scope

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(extent.width);
	viewport.height = static_cast<float>(extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = extent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}