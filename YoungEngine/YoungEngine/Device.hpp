#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <set>
#include <stdexcept>

#include "QueueFamilyIndices.hpp"
#include "DeviceUtils.hpp"
#include "DeviceData.hpp"

//Encapsulates the basic Vulkan types that are to be created once
//and reused during application runtime.
class Device {

public:

	//Constructor
	Device(VkInstance instance, GLFWwindow *window);

	//Copy and Move constructors
	Device(const Device &) = delete;
	Device &operator=(const Device &) = delete;
	Device(Device &&) = delete;
	Device &operator=(Device &&) = delete;

	//Member variables
	VkDevice logical = VK_NULL_HANDLE;
	VkPhysicalDevice physical = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;
	VkCommandPool commandPool = VK_NULL_HANDLE;
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

private:

	void pickPhysicalDevice(VkInstance instance);
	void createSurface(VkInstance instance, GLFWwindow *window);
	void createCommandPool();
};