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

	PipelineDescription::RenderingFormats formats = renderTarget.getRenderingFormats();

	drawItems.clear();
	for (const auto &renderable : scene->getRenderables()) {
		renderable->prepare(formats, SwapChain::active->swapChainExtent);
		renderable->upload(frame);
		renderable->collect(frame, drawItems);
	}

	sortDrawItems(drawItems);
	removeDuplicateFullscreenItems(drawItems);
	firstScreenItem = partitionByTarget(drawItems);
}

void Renderer::recordFrame(VkCommandBuffer commandBuffer, uint32_t imageIndex, size_t frame) {

	//The command buffer has already been begun. First the passes that render into textures, so that
	//the screen's passes after them can sample what they rendered, then the screen.

	recordTexturePasses(commandBuffer, frame);

	renderTarget.setSwapChainImage(imageIndex);
	
	renderTarget.recordBegin(commandBuffer);
	RenderUtility::setViewportAndScissor(commandBuffer, renderTarget.getExtent());
	recordDrawItems(commandBuffer, frame, firstScreenItem, drawItems.size());
	renderTarget.recordEnd(commandBuffer);
}

void Renderer::recordTexturePasses(VkCommandBuffer commandBuffer, size_t frame) {

	//The items before firstScreenItem render into textures. The items of one pass of one material are next to
	//each other and all render into the same target, so each such run is recorded into that target. Since the
	//passes of a material are in order, a pass that samples the target of an earlier pass is recorded after it.

	size_t first = 0;

	while (first < firstScreenItem) {

		const DrawItem &firstItem = drawItems[first];

		size_t last = first + 1;
		while (last < firstScreenItem
			&& drawItems[last].material == firstItem.material
			&& drawItems[last].pass == firstItem.pass) {
			last++;
		}

		const RenderTarget &target = *firstItem.material->getPass(firstItem.pass).getTarget();

		target.recordBegin(commandBuffer);
		RenderUtility::setViewportAndScissor(commandBuffer, target.getExtent());
		recordDrawItems(commandBuffer, frame, first, last);
		target.recordEnd(commandBuffer);

		first = last;
	}
}

void Renderer::recordDrawItems(VkCommandBuffer commandBuffer, size_t frame, size_t first, size_t last) {

	VkDescriptorSet frameSet = frameSets[frame]->getHandle();

	const Pipeline *boundPipeline = nullptr;
	VkDescriptorSet boundMaterialSet = VK_NULL_HANDLE;

	for (size_t i = first; i < last; i++) {

		const DrawItem &item = drawItems[i];

		if (!item.pipeline) {
			throw std::logic_error("A draw item has to have a pipeline!");
		}

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

void Renderer::sortDrawItems(std::vector<DrawItem> &drawItems) {

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
}

void Renderer::removeDuplicateFullscreenItems(std::vector<DrawItem> &drawItems) {

	//Two draw items that work in screen space on the same pass of the same material
	//would be redundant to draw more than once.

	drawItems.erase(
		std::unique(
			drawItems.begin(),
			drawItems.end(),
			[](const DrawItem &a, const DrawItem &b) {
				return a.material && a.material == b.material && a.pass == b.pass
					&& a.material->getPass(a.pass).getSettings().hasFullscreenContext;
			}
		),
		drawItems.end()
	);
}

size_t Renderer::partitionByTarget(std::vector<DrawItem> &drawItems) {

	//First we want to do all passes that render to a separate texture, then on-screen

	auto firstScreenItem = std::stable_partition(
		drawItems.begin(),
		drawItems.end(),
		[](const DrawItem &item) {
			return item.material && item.material->getPass(item.pass).getTarget() != nullptr;
		}
	);

	return static_cast<size_t>(firstScreenItem - drawItems.begin());
}