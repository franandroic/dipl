#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

#include "Device.hpp"
#include "Vertex.hpp"
#include "VertexBufferObject.hpp"
#include "IndexBufferObject.hpp"

class Mesh {

public:
	
	Mesh() = default;

	Mesh(Device *inDevice, std::vector<Vertex> &inVertices, std::vector<uint32_t> &inIndices);

	VertexBufferObject VBO;
	IndexBufferObject IBO;
};