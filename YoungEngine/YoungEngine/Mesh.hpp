#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

#include "Device.hpp"
#include "Vertex.hpp"
#include "VertexBufferObject.hpp"
#include "IndexBufferObject.hpp"

//Describes a mesh by encapsulating its data buffers
class Mesh {

public:

	//Constructor
	Mesh(Device &inDevice, std::vector<Vertex> &inVertices, std::vector<uint32_t> &inIndices);

	//Copy and Move constructors
	Mesh(const Mesh &) = delete;
	Mesh &operator=(const Mesh &) = delete;
	Mesh(Mesh &&) = delete;
	Mesh &operator=(Mesh &&) = delete;

	//Member variables
	VertexBufferObject VBO;
	IndexBufferObject IBO;
};