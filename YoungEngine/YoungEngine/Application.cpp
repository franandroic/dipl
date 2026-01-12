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

	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	
	myDevice = Device(&physicalDevice, &surface);

	mySwapChain = SwapChain(&myDevice, &physicalDevice, &surface, window);

	mySwapChain.createImageViews();

	myRenderPass = RenderPass(&myDevice, physicalDevice, &mySwapChain, msaaSamples);

	myDescription = Description(&myDevice);

	myPipeline = Pipeline(&myDevice, &myRenderPass, &myDescription, msaaSamples);
	
	myCommand = Command(&myDevice, &mySwapChain, &myRenderPass, &myPipeline, &myDescription);
	myCommand.createCommandPool(physicalDevice, surface);

	myCIO = ColorImageObject(&myDevice, mySwapChain.swapChainImageFormat);
	myCIO.createImage(physicalDevice, mySwapChain.swapChainExtent.width, mySwapChain.swapChainExtent.height, msaaSamples);

	colorImageView = mySwapChain.createImageView(myCIO.image, myCIO.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);

	myDIO = DepthImageObject(&myDevice, DeviceUtils::findDepthFormat(physicalDevice));
	myDIO.createImage(physicalDevice, myCommand.commandPool, mySwapChain.swapChainExtent.width, mySwapChain.swapChainExtent.height, msaaSamples);

	depthImageView = mySwapChain.createImageView(myDIO.image, myDIO.depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

	myCommand.createFramebuffers(colorImageView, depthImageView);

	int texWidth, texHeight, texChannels;
	unsigned char *pixels = myLoader.loadImage(&texWidth, &texHeight, &texChannels);
	if (!pixels) {
		throw std::runtime_error("Failed to load texture image!");
	}

	myTIO = TextureImageObject(&myDevice);
	myTIO.createImage(physicalDevice, myCommand.commandPool, pixels, texWidth, texHeight, texChannels);

	myLoader.unloadImage(pixels);

	textureImageView = mySwapChain.createImageView(myTIO.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, myTIO.mipLevels);

	createTextureSampler();
	
	myModelLoader.load(vertices, indices);

	myVBO = VertexBufferObject(
		&myDevice,
		&vertices,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	myVBO.createBuffer(physicalDevice, myCommand.commandPool);

	myIBO = IndexBufferObject(
		&myDevice,
		&indices,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	myIBO.createBuffer(physicalDevice, myCommand.commandPool);

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
		myUBOs[i].createBuffer(physicalDevice);
	}
	
	myDescription.createDescriptorPool();

	myDescription.createDescriptorSets(myUBOs, textureImageView, textureSampler);

	myCommand.createCommandBuffers();

	createSyncObjects();
}

void Application::mainLoop() {

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		drawFrame();
	}

	vkDeviceWaitIdle(myDevice.device);
}

void Application::cleanup() {

	cleanupSwapChain();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(myDevice.device, myUBOs[i].buffer, nullptr);
		vkFreeMemory(myDevice.device, myUBOs[i].bufferMemory, nullptr);
	}

	vkDestroySampler(myDevice.device, textureSampler, nullptr);

	vkDestroyImageView(myDevice.device, textureImageView, nullptr);

	vkDestroyImage(myDevice.device, myTIO.image, nullptr);
	vkFreeMemory(myDevice.device, myTIO.imageMemory, nullptr);

	vkDestroyDescriptorPool(myDevice.device, myDescription.descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(myDevice.device, myDescription.descriptorSetLayout, nullptr);
	vkDestroyPipeline(myDevice.device, myPipeline.graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(myDevice.device, myPipeline.pipelineLayout, nullptr);
	vkDestroyRenderPass(myDevice.device, myRenderPass.renderPass, nullptr);

	vkDestroyBuffer(myDevice.device, myVBO.buffer, nullptr);
	vkFreeMemory(myDevice.device, myVBO.bufferMemory, nullptr);

	vkDestroyBuffer(myDevice.device, myIBO.buffer, nullptr);
	vkFreeMemory(myDevice.device, myIBO.bufferMemory, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(myDevice.device, imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(myDevice.device, renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(myDevice.device, inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(myDevice.device, myCommand.commandPool, nullptr);

	vkDestroyDevice(myDevice.device, nullptr);

	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Application::cleanupSwapChain() {

	vkDestroyImageView(myDevice.device, colorImageView, nullptr);
	vkDestroyImage(myDevice.device, myCIO.image, nullptr);
	vkFreeMemory(myDevice.device, myCIO.imageMemory, nullptr);

	vkDestroyImageView(myDevice.device, depthImageView, nullptr);
	vkDestroyImage(myDevice.device, myDIO.image, nullptr);
	vkFreeMemory(myDevice.device, myDIO.imageMemory, nullptr);

	for (auto framebuffer : myCommand.swapChainFramebuffers) {
		vkDestroyFramebuffer(myDevice.device, framebuffer, nullptr);
	}

	for (auto imageView : mySwapChain.swapChainImageViews) {
		vkDestroyImageView(myDevice.device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(myDevice.device, mySwapChain.swapChain, nullptr);
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

void Application::pickPhysicalDevice() {

	//After listing existing physical devices on the machine, checking for first suitable one
	//and assigning the handle to a member variable.

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto &device : devices) {
		if (DeviceUtils::isDeviceSuitable(device, surface, true)) {
			physicalDevice = device;
			msaaSamples = DeviceUtils::getMaxUsableSampleCount(physicalDevice);
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("Failed to find a suitable GPU!");
	}
}

void Application::createSurface() {

	//Surface is an abstraction of the screen to which to draw onto through the graphics
	//and presentation queues with the swap chain. GLFW provides a function to create it.

	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface!");
	}
}

void Application::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {

	myCommand.recordCommandBuffer(commandBuffer, imageIndex, currentFrame, static_cast<uint32_t>(indices.size()), myVBO.buffer, myIBO.buffer);
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
		if (vkCreateSemaphore(myDevice.device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(myDevice.device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(myDevice.device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
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

	vkWaitForFences(myDevice.device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(myDevice.device, mySwapChain.swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	vkResetFences(myDevice.device, 1, &inFlightFences[currentFrame]);

	vkResetCommandBuffer(myCommand.commandBuffers[currentFrame], 0);
	recordCommandBuffer(myCommand.commandBuffers[currentFrame], imageIndex);

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

	vkDeviceWaitIdle(myDevice.device);

	cleanupSwapChain();

	mySwapChain = SwapChain(&myDevice, &physicalDevice, &surface, window);

	mySwapChain.createImageViews();

	myCIO = ColorImageObject(&myDevice, mySwapChain.swapChainImageFormat);
	myCIO.createImage(physicalDevice, mySwapChain.swapChainExtent.width, mySwapChain.swapChainExtent.height, msaaSamples);

	colorImageView = mySwapChain.createImageView(myCIO.image, myCIO.colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);

	myDIO = DepthImageObject(&myDevice, DeviceUtils::findDepthFormat(physicalDevice));
	myDIO.createImage(physicalDevice, myCommand.commandPool, mySwapChain.swapChainExtent.width, mySwapChain.swapChainExtent.height, msaaSamples);

	depthImageView = mySwapChain.createImageView(myDIO.image, myDIO.depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

	myCommand.createFramebuffers(colorImageView, depthImageView);
}

void Application::createTextureSampler() {

	//Defining members that describe the sampler we use to sample a texture onto an object.

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = VK_LOD_CLAMP_NONE;

	if (vkCreateSampler(myDevice.device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create texture sampler!");
	}
}

bool Application::hasStencilComponent(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}
