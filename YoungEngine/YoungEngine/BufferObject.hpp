#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "DeviceUtils.hpp"
#include "CommandUtils.hpp"

#include "Device.hpp"

//Responsible for handling buffers and their related structures
class BufferObject {

public:

	//Constructor
	BufferObject(
		Device &inDevice,
		VkDeviceSize inSize,
		VkBufferUsageFlags inUsage,
		VkMemoryPropertyFlags inProperties
	);

	//Copy and Move constructors
	BufferObject(const BufferObject &) = delete;
	BufferObject &operator=(const BufferObject &) = delete;
	BufferObject(BufferObject &&) = delete;
	BufferObject &operator=(BufferObject &&) = delete;

	//Member variables
	Device &device;

	const VkDeviceSize size;
	const VkBufferUsageFlags usage;
	const VkMemoryPropertyFlags properties;

	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory bufferMemory = VK_NULL_HANDLE;

	bool isResidentOnGPU();

	void createBuffer();
	void destroyBuffer();
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool commandPool);

};