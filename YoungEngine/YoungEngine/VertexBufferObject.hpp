#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

#include "BufferObject.hpp"
#include "Vertex.hpp"

class VertexBufferObject : public BufferObject {

public:

	VertexBufferObject() = default;

	VertexBufferObject(
		Device *inDevice,
		std::vector<Vertex> *inVertices,
		VkBufferUsageFlags inUsage,
		VkMemoryPropertyFlags inProperties
	) : BufferObject(inDevice, sizeof((*inVertices)[0]) * (*inVertices).size(), inUsage, inProperties) {
			vertices = inVertices;
		}

	void createBuffer(VkPhysicalDevice physicalDevice, VkCommandPool commandPool);

private:

	std::vector<Vertex> *vertices;
};