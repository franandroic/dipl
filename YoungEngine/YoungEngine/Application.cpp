#include "Application.hpp"

Application::Application() : myWindow(WIDTH, HEIGHT, "Vulkan") {}

Application::~Application() {

	if (!myDevice) return;

	for (size_t i = 0; i < imageAvailableSemaphores.size(); i++) {
		vkDestroySemaphore(myDevice->logical, imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(myDevice->logical, renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(myDevice->logical, inFlightFences[i], nullptr);
	}
}

void Application::run() {

	initVulkan();
	mainLoop();
}

void Application::initVulkan() {

	myModelLoader.setPath(MODEL_PATH);
	myImageLoader.setPath(TEXTURE_PATH);
	myLoader.setModelLoader(&myModelLoader);
	myLoader.setImageLoader(&myImageLoader);

	int texWidth, texHeight, texChannels;
	unsigned char *pixels = myLoader.loadImage(&texWidth, &texHeight, &texChannels);
	if (!pixels) {
		throw std::runtime_error("Failed to load texture image!");
	}
	
	myDevice = std::make_unique<Device>(myEngineUtility.instance, myWindow.window);

	mySwapChain = std::make_unique<SwapChain>(*myDevice, myWindow.window);
	mySwapChain->createImageViews();

	myCanvas = std::make_unique<Canvas>(*mySwapChain, pixels, texWidth, texHeight, texChannels);

	myPipeline = std::make_unique<Pipeline>(*myDevice, myCanvas->renderPass, myCanvas->description);

	myLoader.unloadImage(pixels);
	
	myModelLoader.load(vertices, indices);

	myMesh = std::make_unique<Mesh>(*myDevice, vertices, indices);

	myCommand.createCommandBuffers(*myDevice);

	createSyncObjects();
}

void Application::mainLoop() {

	while (!glfwWindowShouldClose(myWindow.window)) {
		glfwPollEvents();
		drawFrame();
	}

	vkDeviceWaitIdle(myDevice->logical);
}

void Application::createSyncObjects() {

	//Creating the semaphores needed to synchronize work on the GPU and
	//fences to sync the CPU and the GPU. If anything is created using the SIGNALED_BIT
	//it means that we need to let the thread pass the first time through.

	//One for each frame we want to be able to fill out before having to wait for the GPU.

	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(myDevice->logical, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(myDevice->logical, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(myDevice->logical, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create sync objects!");
		}
	}
}

void Application::drawFrame() {

	//The main drawing function, where we make the CPU wait for the previous command buffer to finish
	//executing, acquire the swap chain image and use the commands from a command buffer to draw to it.
	//To submit a new queue (which contains render commands) to the GPU we need to wait for the image
	//acquisition to finish. To present the new image back to the swap chain we have to wait for it
	//to be drawn.

	//We are able to render to multiple (MAX_FRAMES_IN_FLIGHT) frames before waiting for previous renders to finish.
	//We need to update the uniform buffer that contains constantly-changing data (transformation matrices) every frame.

	myCanvas->UBOs[currentFrame]->updateBuffer();

	vkWaitForFences(myDevice->logical, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(myDevice->logical, mySwapChain->swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	vkResetFences(myDevice->logical, 1, &inFlightFences[currentFrame]);

	vkResetCommandBuffer(myCommand.commandBuffers[currentFrame], 0);
	myCommand.recordCommandBuffer(
		myCommand.commandBuffers[currentFrame],
		imageIndex,
		currentFrame,
		static_cast<uint32_t>(indices.size()),
		myMesh->VBO.buffer,
		myMesh->IBO.buffer,
		myCanvas->FBO,
		myCanvas->renderPass,
		*myPipeline,
		myCanvas->description);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &myCommand.commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};

	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(myDevice->graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { mySwapChain->swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(myDevice->presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || myWindow.framebufferResized) {
		myWindow.framebufferResized = false;
		recreateSwapChain();
	} else if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Application::recreateSwapChain() {

	//It's necessary to recreate the swap chain and connected structures when the window resizes
	//or minimizes.

	int width = 0;
	int height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(myWindow.window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(myDevice->logical);

	mySwapChain->recreate(myWindow.window);
	mySwapChain->createImageViews();

	myCanvas->FBO.recreateFramebuffers(myCanvas->renderPass);
}
