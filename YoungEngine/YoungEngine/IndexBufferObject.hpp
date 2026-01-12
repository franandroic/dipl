#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

#include "BufferObject.hpp"

class IndexBufferObject : public BufferObject {

public:

	IndexBufferObject() = default;

	IndexBufferObject(
		Device *inDevice,
		std::vector<uint32_t> *inIndices,
		VkBufferUsageFlags inUsage,
		VkMemoryPropertyFlags inProperties
	) : BufferObject(inDevice, sizeof((*inIndices)[0]) * (*inIndices).size(), inUsage, inProperties) {
			indices = inIndices;
		}

	void createBuffer(VkCommandPool commandPool);

private:

	std::vector<uint32_t> *indices;
};