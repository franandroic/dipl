#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <set>
#include <stdexcept>

#include "QueueFamilyIndices.hpp"
#include "DeviceUtils.hpp"
#include "DeviceData.hpp"

class Device {

public:

	Device() = default;

	Device(VkPhysicalDevice *physicalDevice, VkSurfaceKHR *surface);
	
	VkDevice device;

	VkQueue graphicsQueue;
	
	VkQueue presentQueue;
};