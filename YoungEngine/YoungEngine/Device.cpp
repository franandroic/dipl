#include "Device.hpp"

Device::Device(VkInstance inInstance, GLFWwindow *window) : instance(inInstance) {

	createSurface(instance, window);
	pickPhysicalDevice(instance);

	//To create a logical device there needs to exist a physical device, a number of queues
	//(with known indices) and an optional validation layer. We fill the create info with the
	//needed data before creating the logical device and getting the handles for its queues.

	QueueFamilyIndices indices = DeviceUtils::findQueueFamilies(physical, surface);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	//TODO: Look into sample shading

	VkPhysicalDeviceVulkan13Features features13{};
	features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	features13.dynamicRendering = VK_TRUE;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pNext = &features13;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physical, &createInfo, nullptr, &logical) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create logical device!");
	}

	vkGetDeviceQueue(logical, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(logical, indices.presentFamily.value(), 0, &presentQueue);

	createCommandPool();
	createDescriptorPool();
}

Device::~Device() {

	if (logical != VK_NULL_HANDLE) {
		vkDeviceWaitIdle(logical);
		vkDestroyDescriptorPool(logical, descriptorPool, nullptr);
		vkDestroyCommandPool(logical, commandPool, nullptr);
		vkDestroyDevice(logical, nullptr);
	}

	if (surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(instance, surface, nullptr);
	}
}

void Device::pickPhysicalDevice(VkInstance instance) {

	//After listing existing physical devices on the machine, checking for first suitable one
	//and assigning the handle to a member variable.

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto &d : devices) {
		if (DeviceUtils::isDeviceSuitable(d, surface, true)) {
			physical = d;
			msaaSamples = DeviceUtils::getMaxUsableSampleCount(physical);
			break;
		}
	}

	if (physical == VK_NULL_HANDLE) {
		throw std::runtime_error("Failed to find a suitable GPU!");
	}
}

void Device::createSurface(VkInstance instance, GLFWwindow *window) {

	//Surface is an abstraction of the screen to which to draw onto through the graphics
	//and presentation queues with the swap chain. GLFW provides a function to create it.

	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface!");
	}
}

void Device::createCommandPool() {

	//Creating a command pool as a place to store command buffers.
	//It is submitted to a queue, so we need to pass which queue it is to be submitted to.

	QueueFamilyIndices queueFamilyIndices = DeviceUtils::findQueueFamilies(physical, surface);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(logical, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create command pool!");
	}
}

void Device::createDescriptorPool() {

	//Creating the descriptor pool all descriptor sets are allocated from, with a pool size for each
	//descriptor type. The sizes are the total number of descriptors of that type across all the sets
	//of the pool. The free flag lets each DescriptorSet return its own set to the pool when destroyed
	//(without it, a pool can only be reset as a whole).

	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = MAX_UNIFORM_BUFFER_DESCRIPTORS;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = MAX_COMBINED_IMAGE_SAMPLER_DESCRIPTORS;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = MAX_DESCRIPTOR_SETS;

	if (vkCreateDescriptorPool(logical, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor pool!");
	}
}