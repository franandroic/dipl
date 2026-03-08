#include "Application.hpp"

void Application::run() {

	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void Application::initWindow() {

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
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

	createInstance();
	setupDebugMessenger();
	
	myDevice = Device(instance, window);

	mySwapChain = SwapChain(&myDevice, window);
	mySwapChain.createImageViews();
	myRenderPass = RenderPass(&myDevice, &mySwapChain);

	myDescription = Description(&myDevice);

	myPipeline = Pipeline(&myDevice, &myRenderPass, &myDescription);

	myFBO = FrameBufferObject(&mySwapChain, &myRenderPass, pixels, texWidth, texHeight, texChannels);
	myCommand = Command(&myFBO, &myRenderPass, &myPipeline, &myDescription);
	myLoader.unloadImage(pixels);
	
	myModelLoader.load(vertices, indices);

	myVBO = VertexBufferObject(
		&myDevice,
		&vertices,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	myVBO.createBuffer(myDevice.commandPool);

	myIBO = IndexBufferObject(
		&myDevice,
		&indices,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	myIBO.createBuffer(myDevice.commandPool);

	myUBOs.resize(MAX_FRAMES_IN_FLIGHT);
	myUBop = UniformBufferOperator(mySwapChain.swapChainExtent.width / (float)mySwapChain.swapChainExtent.height);
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		myUBOs[i] = UniformBufferObject(
			&myDevice,
			&myUBdata,
			&myUBop,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		myUBOs[i].createBuffer();
	}
	
	myDescription.createDescriptorPool();
	myDescription.createDescriptorSets(myUBOs, myFBO.TIO.imageView, myFBO.TIO.sampler);

	myCommand.createCommandBuffers();

	createSyncObjects();
}

void Application::mainLoop() {

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		drawFrame();
	}

	vkDeviceWaitIdle(myDevice.logical);
}

void Application::cleanup() {

	cleanupSwapChain();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(myDevice.logical, myUBOs[i].buffer, nullptr);
		vkFreeMemory(myDevice.logical, myUBOs[i].bufferMemory, nullptr);
	}

	vkDestroySampler(myDevice.logical, myFBO.TIO.sampler, nullptr);

	vkDestroyImageView(myDevice.logical, myFBO.TIO.imageView, nullptr);

	vkDestroyImage(myDevice.logical, myFBO.TIO.image, nullptr);
	vkFreeMemory(myDevice.logical, myFBO.TIO.imageMemory, nullptr);

	vkDestroyDescriptorPool(myDevice.logical, myDescription.descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(myDevice.logical, myDescription.descriptorSetLayout, nullptr);
	vkDestroyPipeline(myDevice.logical, myPipeline.graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(myDevice.logical, myPipeline.pipelineLayout, nullptr);
	vkDestroyRenderPass(myDevice.logical, myRenderPass.renderPass, nullptr);

	vkDestroyBuffer(myDevice.logical, myVBO.buffer, nullptr);
	vkFreeMemory(myDevice.logical, myVBO.bufferMemory, nullptr);

	vkDestroyBuffer(myDevice.logical, myIBO.buffer, nullptr);
	vkFreeMemory(myDevice.logical, myIBO.bufferMemory, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(myDevice.logical, imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(myDevice.logical, renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(myDevice.logical, inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(myDevice.logical, myDevice.commandPool, nullptr);

	vkDestroyDevice(myDevice.logical, nullptr);

	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(instance, myDevice.surface, nullptr);
	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Application::cleanupSwapChain() {

	vkDestroyImageView(myDevice.logical, myFBO.CIO.imageView, nullptr);
	vkDestroyImage(myDevice.logical, myFBO.CIO.image, nullptr);
	vkFreeMemory(myDevice.logical, myFBO.CIO.imageMemory, nullptr);

	vkDestroyImageView(myDevice.logical, myFBO.DIO.imageView, nullptr);
	vkDestroyImage(myDevice.logical, myFBO.DIO.image, nullptr);
	vkFreeMemory(myDevice.logical, myFBO.DIO.imageMemory, nullptr);

	for (auto framebuffer : myFBO.framebuffers) {
		vkDestroyFramebuffer(myDevice.logical, framebuffer, nullptr);
	}

	for (auto imageView : mySwapChain.swapChainImageViews) {
		vkDestroyImageView(myDevice.logical, imageView, nullptr);
	}

	vkDestroySwapchainKHR(myDevice.logical, mySwapChain.swapChain, nullptr);
}

void Application::createInstance() {

	//To create a Vulkan instance it's necessary to fill out info about the application and
	//info about the instance, which requires checking for and enabling the required extensions.
	//Before creating the instance we create a special debug messenger for this object/code.

	VkApplicationInfo appInfo{};

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Application";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Young Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = DeviceUtils::getRequiredExtensions(true);

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (enableValidationLayers && !DeviceUtils::checkValidationLayerSupport(true)) {
		throw std::runtime_error("Validation layers requested, but not available!");
	}

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
	} else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create instance!");
	}
}

void Application::setupDebugMessenger() {

	//Creating a debug messenger after populating it with needed data.

	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo {};
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("Failed to set up debug messenger!");
	}
}

void Application::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {

	//Populating debug messenger creation info.

	createInfo = {};

	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;
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
		if (vkCreateSemaphore(myDevice.logical, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(myDevice.logical, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(myDevice.logical, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
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

	myUBOs[currentFrame].updateBuffer();

	vkWaitForFences(myDevice.logical, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(myDevice.logical, mySwapChain.swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	vkResetFences(myDevice.logical, 1, &inFlightFences[currentFrame]);

	vkResetCommandBuffer(myCommand.commandBuffers[currentFrame], 0);
	myCommand.recordCommandBuffer(myCommand.commandBuffers[currentFrame], imageIndex, currentFrame, static_cast<uint32_t>(indices.size()), myVBO.buffer, myIBO.buffer);

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

	if (vkQueueSubmit(myDevice.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { mySwapChain.swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(myDevice.presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
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
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(myDevice.logical);

	cleanupSwapChain();

	mySwapChain = SwapChain(&myDevice, window);

	mySwapChain.createImageViews();

	myFBO.recreateFramebuffers(&myRenderPass);
}
