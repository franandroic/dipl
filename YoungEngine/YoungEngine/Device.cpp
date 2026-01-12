#include "Device.hpp"

Device::Device(VkInstance instance, GLFWwindow *window) {

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

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
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