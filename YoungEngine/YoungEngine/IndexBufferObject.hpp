#pragma once
#include "GLFWConfig.hpp"

#include <vector>

#include "BufferObject.hpp"

//Buffer object that handles index data
class IndexBufferObject : public BufferObject {

public:

	//Construction
	IndexBufferObject(
		const std::vector<uint32_t> &inIndices,
		VkBufferUsageFlags inUsage,
		VkMemoryPropertyFlags inProperties
	) : BufferObject(sizeof(inIndices[0]) * inIndices.size(), inUsage, inProperties),
		indexCount(static_cast<uint32_t>(inIndices.size())) {}

	//Member variables
	const uint32_t indexCount;

	void createBuffer(const std::vector<uint32_t> &indices);
};