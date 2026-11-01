#pragma once
#include "GLFWConfig.hpp"

#include <set>
#include <stdexcept>
#include <array>
#include <assert.h>

#include "QueueFamilyIndices.hpp"
#include "DeviceUtils.hpp"
#include "DeviceData.hpp"

/*
Encapsulates data and structures that are unique and intact during runtime.
Members are accessed globally through the static variable.

Renderer accesses queues when rendering and presenting,
command pool is accessed when creating a Command and
descriptor pool is accessed when creating a DescriptorSet.
*/
class Device {

public:

	//Construction
	Device(VkInstance inInstance, GLFWwindow *window);
	~Device();

	//Copy and Move constructors
	Device(const Device &) = delete;
	Device &operator=(const Device &) = delete;
	Device(Device &&) = delete;
	Device &operator=(Device &&) = delete;

	//Global access
	static inline Device *active = nullptr;

	//Member variables
	VkDevice logical = VK_NULL_HANDLE;
	VkPhysicalDevice physical = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;
	VkCommandPool commandPool = VK_NULL_HANDLE;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

private:

	VkInstance instance = VK_NULL_HANDLE;

	void pickPhysicalDevice(VkInstance instance);
	void createSurface(VkInstance instance, GLFWwindow *window);
	void createCommandPool();
	void createDescriptorPool();
};