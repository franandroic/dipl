#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

#include "BufferObject.hpp"
#include "Vertex.hpp"

//Buffer object that handles vertex data
class VertexBufferObject : public BufferObject {

public:
	
	//Constructor
	VertexBufferObject(
		Device &inDevice,
		const std::vector<Vertex> &inVertices,
		VkBufferUsageFlags inUsage,
		VkMemoryPropertyFlags inProperties
	) : BufferObject(inDevice, sizeof((inVertices)[0]) * (inVertices).size(), inUsage, inProperties) {}

	void createBuffer(VkCommandPool commandPool, const std::vector<Vertex> &vertices);
};