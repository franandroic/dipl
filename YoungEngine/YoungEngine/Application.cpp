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
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createDescriptorSetLayout();
	createGraphicsPipeline();
	createCommandPool();
	createColorResources();
	createDepthResources();
	createFramebuffers();
	createTextureImage();
	createTextureImageView();
	createTextureSampler();
	loadModel();
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();
	createSyncObjects();
}

void Application::mainLoop() {

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		drawFrame();
	}

	vkDeviceWaitIdle(device);
}

void Application::cleanup() {

	cleanupSwapChain();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(device, uniformBuffers[i], nullptr);
		vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
	}

	vkDestroySampler(device, textureSampler, nullptr);

	vkDestroyImageView(device, textureImageView, nullptr);

	vkDestroyImage(device, textureImage, nullptr);
	vkFreeMemory(device, textureImageMemory, nullptr);

	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);

	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkFreeMemory(device, vertexBufferMemory, nullptr);

	vkDestroyBuffer(device, indexBuffer, nullptr);
	vkFreeMemory(device, indexBufferMemory, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(device, commandPool, nullptr);

	vkDestroyDevice(device, nullptr);

	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Application::cleanupSwapChain() {

	vkDestroyImageView(device, colorImageView, nullptr);
	vkDestroyImage(device, colorImage, nullptr);
	vkFreeMemory(device, colorImageMemory, nullptr);

	vkDestroyImageView(device, depthImageView, nullptr);
	vkDestroyImage(device, depthImage, nullptr);
	vkFreeMemory(device, depthImageMemory, nullptr);

	for (auto framebuffer : swapChainFramebuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}

	for (auto imageView : swapChainImageViews) {
		vkDestroyImageView(device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(device, swapChain, nullptr);
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

void Application::createLogicalDevice() {

	myDevice = Device(&physicalDevice, &surface);
	device = myDevice.device;
	graphicsQueue = myDevice.graphicsQueue;
	presentQueue = myDevice.presentQueue;
}

void Application::createSurface() {

	//Surface is an abstraction of the screen to which to draw onto through the graphics
	//and presentation queues with the swap chain. GLFW provides a function to create it.

	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface!");
	}
}

void Application::createSwapChain() {

	mySwapChain = SwapChain(&myDevice, &physicalDevice, &surface, window);
	swapChain = mySwapChain.swapChain;
	swapChainImages = mySwapChain.swapChainImages;
	swapChainImageFormat = mySwapChain.swapChainImageFormat;
	swapChainExtent = mySwapChain.swapChainExtent;
}

void Application::createImageViews() {

	mySwapChain.createImageViews();
	swapChainImageViews = mySwapChain.swapChainImageViews;
}

void Application::createDescriptorSetLayout() {

	myDescription = Description(&myDevice);
	descriptorSetLayout = myDescription.descriptorSetLayout;
}

void Application::createDescriptorPool() {

	myDescription.createDescriptorPool();
	descriptorPool = myDescription.descriptorPool;
}

void Application::createDescriptorSets() {

	myDescription.createDescriptorSets(uniformBuffers, textureImageView, textureSampler);
	descriptorSets = myDescription.descriptorSets;
}

void Application::createRenderPass() {

	myRenderPass = RenderPass(&myDevice, physicalDevice, &mySwapChain, msaaSamples);
	renderPass = myRenderPass.renderPass;
}

void Application::createGraphicsPipeline() {

	myPipeline = Pipeline(&myDevice, &myRenderPass, &myDescription, msaaSamples);
	pipelineLayout = myPipeline.pipelineLayout;
	graphicsPipeline = myPipeline.graphicsPipeline;
}

void Application::createCommandPool() {

	myCommand = Command(&myDevice, &mySwapChain, &myRenderPass, &myPipeline, &myDescription);
	myCommand.createCommandPool(physicalDevice, surface);
	commandPool = myCommand.commandPool;
}

void Application::createFramebuffers() {

	myCommand.createFramebuffers(colorImageView, depthImageView);
	swapChainFramebuffers = myCommand.swapChainFramebuffers;
}

void Application::createCommandBuffers() {

	myCommand.createCommandBuffers();
	commandBuffers = myCommand.commandBuffers;
}

void Application::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {

	myCommand.recordCommandBuffer(commandBuffer, imageIndex, currentFrame, static_cast<uint32_t>(indices.size()), vertexBuffer, indexBuffer);
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
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
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

	updateUniformBuffer(currentFrame);

	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	vkResetFences(device, 1, &inFlightFences[currentFrame]);

	vkResetCommandBuffer(commandBuffers[currentFrame], 0);
	recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};

	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(presentQueue, &presentInfo);
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

	vkDeviceWaitIdle(device);

	cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createColorResources();
	createDepthResources();
	createFramebuffers();
}

void Application::createVertexBuffer() {

	myVBO = VertexBufferObject(
		&myDevice,
		&vertices,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	myVBO.createBuffer(physicalDevice, commandPool);

	vertexBuffer = myVBO.buffer;
	vertexBufferMemory = myVBO.bufferMemory;
}

void Application::createIndexBuffer() {

	myIBO = IndexBufferObject(
		&myDevice,
		&indices,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	myIBO.createBuffer(physicalDevice, commandPool);

	indexBuffer = myIBO.buffer;
	indexBufferMemory = myIBO.bufferMemory;
}

void Application::createUniformBuffers() {

	//Creating and mapping a uniform buffer for each frame (imageView) we want to be able
	//to draw to before presenting.

	myUBOs.resize(MAX_FRAMES_IN_FLIGHT);

	uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	myUBop = UniformBufferOperator(swapChainExtent.width / (float)swapChainExtent.height);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

		myUBOs[i] = UniformBufferObject(
			&myDevice,
			&myUBdata,
			&myUBop,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		myUBOs[i].createBuffer(physicalDevice);

		uniformBuffers[i] = myUBOs[i].buffer;
		uniformBuffersMemory[i] = myUBOs[i].bufferMemory;
		uniformBuffersMapped[i] = myUBOs[i].bufferMapped;
	}
}

void Application::updateUniformBuffer(uint32_t currentImage) {

	myUBOs[currentImage].updateBuffer();
}

void Application::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory) {

	//A generic image creation function that works similar to the buffer creation function.

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = numSamples;
	imageInfo.flags = 0;

	if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = DeviceUtils::findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate image memory!");
	}

	vkBindImageMemory(device, image, imageMemory, 0);
}

void Application::createTextureImage() {

	//Similar to buffer creation (regarding the use of two buffers),
	//with the use of the stb library to load an image from disk and some additional
	//helper functions for transitioning image layouts and copying buffer to image.
	//In order to transfer the pixel data from a CPU buffer to a staging buffer, and then
	//from the staging buffer to an image object, we need to have the image in proper layouts.

	int texWidth, texHeight, texChannels;
	
	unsigned char *pixels = myLoader.loadImage(&texWidth, &texHeight, &texChannels);

	mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels) {
		throw std::runtime_error("Failed to load texture image!");
	}

	BufferObject stagingBufferObject(
		&myDevice,
		imageSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	stagingBufferObject.createBuffer(physicalDevice);

	void *data;
	vkMapMemory(device, stagingBufferObject.bufferMemory, 0, imageSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(device, stagingBufferObject.bufferMemory);

	myLoader.unloadImage(pixels);

	createImage(texWidth,
				texHeight,
				mipLevels,
				VK_SAMPLE_COUNT_1_BIT,
				VK_FORMAT_R8G8B8A8_SRGB,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				textureImage,
				textureImageMemory);

	ImageUtils::transitionImageLayout(device, commandPool, graphicsQueue, textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		ImageUtils::copyBufferToImage(device, commandPool, graphicsQueue, stagingBufferObject.buffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	//transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	
	generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels);

	vkDestroyBuffer(device, stagingBufferObject.buffer, nullptr);
	vkFreeMemory(device, stagingBufferObject.bufferMemory, nullptr);
}

void Application::createTextureImageView() {

	textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
}

VkImageView Application::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {

	return mySwapChain.createImageView(image, format, aspectFlags, mipLevels);
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

	if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create texture sampler!");
	}
}

void Application::createDepthResources() {

	VkFormat depthFormat = DeviceUtils::findDepthFormat(physicalDevice);

	createImage(swapChainExtent.width,
				swapChainExtent.height,
				1,
				msaaSamples,
				depthFormat,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				depthImage,
				depthImageMemory);

	depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

	ImageUtils::transitionImageLayout(device, commandPool, graphicsQueue, depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

bool Application::hasStencilComponent(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void Application::loadModel() {

	myModelLoader.load(vertices, indices);
}

void Application::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) {

	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
		throw std::runtime_error("Texture image format does not support linear blitting!");
	}

	VkCommandBuffer commandBuffer = CommandUtils::beginSingleTimeCommands(device, commandPool);

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	int32_t mipWidth = texWidth;
	int32_t mipHeight = texHeight;

	for (uint32_t i = 1; i < mipLevels; i++) {
		
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
							 VK_PIPELINE_STAGE_TRANSFER_BIT,
							 VK_PIPELINE_STAGE_TRANSFER_BIT,
							 0,
							 0,
							 nullptr,
							 0,
							 nullptr,
							 1,
							 &barrier);
		
		VkImageBlit blit{};
		blit.srcOffsets[0] = {	0, 0, 0	};
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(commandBuffer,
					   image,
					   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					   image,
					   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					   1,
					   &blit,
					   VK_FILTER_LINEAR);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
							 VK_PIPELINE_STAGE_TRANSFER_BIT,
							 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
							 0,
							 0,
							 nullptr,
							 0,
							 nullptr,
							 1,
							 &barrier);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(commandBuffer,
						 VK_PIPELINE_STAGE_TRANSFER_BIT,
						 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
						 0,
						 0,
						 nullptr,
						 0,
						 nullptr,
						 1,
						 &barrier);

	CommandUtils::endSingleTimeCommands(commandBuffer, device, commandPool, graphicsQueue);
}

void Application::createColorResources() {

	VkFormat colorFormat = swapChainImageFormat;

	createImage(swapChainExtent.width,
				swapChainExtent.height,
				1,
				msaaSamples,
				colorFormat,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				colorImage,
				colorImageMemory);

	colorImageView = createImageView(colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}