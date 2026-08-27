#include "Mesh.hpp"

Mesh::Mesh(Device &inDevice, std::vector<Vertex> inVertices, std::vector<uint32_t> inIndices)
	: vertices(std::move(inVertices)),
	indices(std::move(inIndices)),
	VBO(inDevice, vertices, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
	IBO(inDevice, indices, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
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
