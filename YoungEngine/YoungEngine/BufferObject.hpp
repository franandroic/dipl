#pragma once
#include "GLFWConfig.hpp"

#include "DeviceUtility.hpp"
#include "CommandUtility.hpp"

#include "Device.hpp"

//Responsible for handling buffers and their related structures
class BufferObject {

public:

	//Construction
	BufferObject(
		VkDeviceSize inSize,
		VkBufferUsageFlags inUsage,
		VkMemoryPropertyFlags inProperties
	);
	~BufferObject();

	//Copy and Move constructors
	BufferObject(const BufferObject &) = delete;
	BufferObject &operator=(const BufferObject &) = delete;
	BufferObject(BufferObject &&) = delete;
	BufferObject &operator=(BufferObject &&) = delete;

	//Member variables
	const VkDeviceSize size;
	const VkBufferUsageFlags usage;
	const VkMemoryPropertyFlags properties;

	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory bufferMemory = VK_NULL_HANDLE;

	bool isResidentOnGPU();

	void createBuffer();
	void destroyBuffer();
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

};