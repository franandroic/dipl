#include "StructureBufferObject.hpp"

StructureBufferObject::StructureBufferObject(Device *inDevice, std::vector<Vertex> &inVertices, std::vector<uint32_t> &inIndices) {

	VBO = VertexBufferObject(
		inDevice,
		&inVertices,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	VBO.createBuffer(inDevice->commandPool);

	IBO = IndexBufferObject(
		inDevice,
		&inIndices,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	IBO.createBuffer(inDevice->commandPool);
}