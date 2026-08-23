#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

#include "BufferObject.hpp"

//Buffer object that handles index data
class IndexBufferObject : public BufferObject {

public:

	//Constructor
	IndexBufferObject(
		Device &inDevice,
		const std::vector<uint32_t> &inIndices,
		VkBufferUsageFlags inUsage,
		VkMemoryPropertyFlags inProperties
	) : BufferObject(inDevice, sizeof(inIndices[0]) * inIndices.size(), inUsage, inProperties),
		indexCount(static_cast<uint32_t>(inIndices.size())) {}

	//Member variables
	const uint32_t indexCount;

	void createBuffer(VkCommandPool commandPool, const std::vector<uint32_t> &indices);
};