#include "Mesh.hpp"

namespace {
	constexpr VkBufferUsageFlags MESH_VERTEX_BUFFER_USAGE_BITS = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	constexpr VkBufferUsageFlags MESH_INDEX_BUFFER_USAGE_BITS = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	constexpr VkMemoryPropertyFlags MESH_MEMORY_PROPERTY_BITS = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
}

Mesh::Mesh(Device &inDevice, std::vector<Vertex> inVertices, std::vector<uint32_t> inIndices)
	: vertices(std::move(inVertices)),
	indices(std::move(inIndices)),
	VBO(inDevice, vertices, MESH_VERTEX_BUFFER_USAGE_BITS, MESH_MEMORY_PROPERTY_BITS),
	IBO(inDevice, indices, MESH_INDEX_BUFFER_USAGE_BITS, MESH_MEMORY_PROPERTY_BITS),
	device(inDevice) {

	createBuffers();
}

bool Mesh::isResidentOnGPU() {

	return VBO.isResidentOnGPU() && IBO.isResidentOnGPU();
}

void Mesh::createBuffers() {
	
	if (isResidentOnGPU()) return;

	VBO.createBuffer(device.commandPool, vertices);
	IBO.createBuffer(device.commandPool, indices);
}

void Mesh::destroyBuffers() {

	VBO.destroyBuffer();
	IBO.destroyBuffer();
}