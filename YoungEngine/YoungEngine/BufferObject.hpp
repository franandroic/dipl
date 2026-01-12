#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "DeviceUtils.hpp"
#include "CommandUtils.hpp"

#include "Device.hpp"

class BufferObject {

public:

	BufferObject() = default;

	BufferObject(
		Device *inDevice,
		VkDeviceSize inSize,
		VkBufferUsageFlags inUsage,
		VkMemoryPropertyFlags inProperties
	);

	VkDeviceSize size;

	VkBufferUsageFlags usage;

	VkMemoryPropertyFlags properties;

	VkBuffer buffer;

	VkDeviceMemory bufferMemory;

	Device *device;

	void createBuffer();

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool commandPool);

};