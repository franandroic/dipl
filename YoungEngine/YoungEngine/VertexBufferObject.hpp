#pragma once
#include "GLFWConfig.hpp"

#include <vector>

#include "BufferObject.hpp"
#include "Vertex.hpp"

//Buffer object that handles vertex data
class VertexBufferObject : public BufferObject {

public:
	
	//Construction
	VertexBufferObject(
		const std::vector<Vertex> &inVertices,
		VkBufferUsageFlags inUsage,
		VkMemoryPropertyFlags inProperties
	) : BufferObject(sizeof((inVertices)[0]) * (inVertices).size(), inUsage, inProperties) {}

	void createBuffer(const std::vector<Vertex> &vertices);
};