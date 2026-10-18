#include "Renderer.hpp"

Renderer::Renderer(Device &inDevice, SwapChain &inSwapChain, Window &inWindow, Camera &inCamera, size_t framesInFlight)
	: device(inDevice),
	swapChain(inSwapChain),
	window(inWindow),
	camera(inCamera),
	renderTarget(inSwapChain),
	sync(inDevice, framesInFlight),
	command(inDevice, framesInFlight) {
}

void Renderer::addDrawer(std::unique_ptr<Drawer> drawer) {

	drawers.push_back(std::move(drawer));
}

void Renderer::render(bool externalResizeRequested) {

	sync.stop(currentFrame);

	updateFrame(currentFrame);

	uint32_t imageIndex;
	SwapChain::AcquireResult acquireResult = swapChain.acquireNextImage(
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

	bool needsRecreate = swapChain.present(sync.renderFinished(currentFrame), imageIndex);
	if (needsRecreate || externalResizeRequested) {
		recreateSwapChain();
	}

	currentFrame = (currentFrame + 1) % sync.count();
}

void Renderer::updateFrame(size_t frame) {

	for (auto &drawer : drawers) {
		drawer->update(frame, camera);
	}
}

void Renderer::recordFrame(VkCommandBuffer commandBuffer, uint32_t imageIndex, size_t frame) {

	//The command buffer has already been begun. The parts that happen once per frame are here:
	//the transitions of the images and the rendering scope (the attachments are cleared when it begins).
	//Everything in between is recorded by the drawers into the same scope.

	//Without a render pass, nothing transitions the layouts of the attachments or synchronizes
	//access to them for us. That's why the rendering is bracketed by explicit image barriers.

	VkImage swapChainImage = swapChain.swapChainImages[imageIndex];
	VkImageView swapChainImageView = swapChain.swapChainImageViews[imageIndex];
	VkExtent2D extent = swapChain.swapChainExtent;

	VkImageAspectFlags depthAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	if (ImageUtils::hasStencilComponent(renderTarget.depthFormat())) {
		depthAspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	//BARRIERS BEFORE RENDERING
	//All three images are written from scratch (cleared, or fully resolved into)

	//The swap chain image receives the resolved result
	ImageUtils::recordImageLayoutTransition(
		commandBuffer,
		swapChainImage,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	);

	//The multisampled color image and the depth image are shared between the frames in flight
	ImageUtils::recordImageLayoutTransition(
		commandBuffer,
		renderTarget.CIO.image,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	);

	ImageUtils::recordImageLayoutTransition(
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

	//Viewport and scissor are dynamic state of every pipeline, so they're set once for all the drawers
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

	for (auto &drawer : drawers) {
		drawer->record(commandBuffer, frame);
	}

	vkCmdEndRendering(commandBuffer);

	//BARRIER AFTER RENDERING
	//The resolved swap chain image goes to the layout required for presentation.
	//The render pass used to do this through the finalLayout of the resolve attachment.
	ImageUtils::recordImageLayoutTransition(
		commandBuffer,
		swapChainImage,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0
	);
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

	if (vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, inFlight) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer!");
	}
}

void Renderer::recreateSwapChain() {

	int width = 0;
	int height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window.window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(device.logical);

	swapChain.recreate(window.window);
	swapChain.createImageViews();

	renderTarget.recreateAttachments();

	camera.setViewportExtent(swapChain.swapChainExtent.width, swapChain.swapChainExtent.height);
}