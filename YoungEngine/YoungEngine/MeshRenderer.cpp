#include "MeshRenderer.hpp"

MeshRenderer::MeshRenderer(
	Device &inDevice,
	SwapChain &inSwapChain,
	Window &inWindow,
	Canvas &inCanvas,
	Pipeline &inPipeline,
	Mesh &inMesh,
	Command &inCommand,
	Camera &inCamera,
	uint32_t inIndexCount,
	size_t inFramesInFlight
) : Renderer(inDevice, inSwapChain, inFramesInFlight),
	window(inWindow),
	canvas(inCanvas),
	pipeline(inPipeline),
	mesh(inMesh),
	command(inCommand),
	camera(inCamera),
	indexCount(inIndexCount) {
}

void MeshRenderer::updateFrame(size_t frame) {

	//The object's uniforms are updated by its own operator, while the camera's data
	//goes into the camera uniform buffer of this frame.

	canvas.uniformity.update(frame);

	Camera::UniformBlock cameraBlock = camera.getUniformBlock();
	canvas.UBOs[frame]->updateBuffer(&cameraBlock, sizeof(cameraBlock));
}

void MeshRenderer::recordFrame(uint32_t imageIndex, size_t frame) {

	vkResetCommandBuffer(command.commandBuffers[frame], 0);

	command.recordCommandBuffer(
		command.commandBuffers[frame],
		imageIndex,
		static_cast<uint32_t>(frame),
		indexCount,
		mesh.VBO.buffer,
		mesh.IBO.buffer,
		pipeline,
		canvas.description,
		canvas.uniformity
	);
}

void MeshRenderer::submitFrame(size_t frame, VkSemaphore imageAvailable, VkSemaphore renderFinished, VkFence inFlight) {

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailable };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &command.commandBuffers[frame];

	VkSemaphore signalSemaphores[] = { renderFinished };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, inFlight) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer!");
	}
}

void MeshRenderer::recreateSwapChain() {

	int width = 0;
	int height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window.window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(device.logical);

	swapChain.recreate(window.window);
	swapChain.createImageViews();

	pipeline.renderTarget.recreateAttachments();
	
	camera.setViewportExtent(swapChain.swapChainExtent.width, swapChain.swapChainExtent.height);
}