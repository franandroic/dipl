#pragma once
#include "GLFWConfig.hpp"

#include <cstddef>

#include "BufferObject.hpp"

//Thin wrapper around a buffer used as a uniform buffer: persistently mapped, host-visible memory
//that the CPU can write to. It knows nothing about what it contains.
class UniformBufferObject : public BufferObject {

public:

	//Construction
	UniformBufferObject(
		Device &inDevice,
		VkDeviceSize inSize,
		VkBufferUsageFlags inUsage,
		VkMemoryPropertyFlags inProperties
	) : BufferObject(inDevice, inSize, inUsage, inProperties) {}

	//Member variables
	void *bufferMapped = nullptr;

	void createBuffer();
	void updateBuffer(const void *data, size_t dataSize);
};