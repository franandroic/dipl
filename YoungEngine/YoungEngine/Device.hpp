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

	Device(VkInstance instance, GLFWwindow *window);
	
	VkDevice logical;

	VkPhysicalDevice physical;

	VkSurfaceKHR surface;

	VkQueue graphicsQueue;
	
	VkQueue presentQueue;

	VkSampleCountFlagBits msaaSamples;

private:

	void pickPhysicalDevice(VkInstance instance);

	void createSurface(VkInstance instance, GLFWwindow *window);
};