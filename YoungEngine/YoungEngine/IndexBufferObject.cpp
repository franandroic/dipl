#include "IndexBufferObject.hpp"

void IndexBufferObject::createBuffer(VkCommandPool commandPool, const std::vector<uint32_t> &indices) {

	//Same as the vertex memory, but for the index buffer.

	BufferObject stagingBufferObject(
		device,
		size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	stagingBufferObject.createBuffer();

	void *data;
	vkMapMemory(device.logical, stagingBufferObject.bufferMemory, 0, size, 0, &data);
	memcpy(data, indices.data(), (size_t)size);
	vkUnmapMemory(device.logical, stagingBufferObject.bufferMemory);

	BufferObject::createBuffer();

	copyBuffer(stagingBufferObject.buffer, buffer, size, commandPool);

	stagingBufferObject.destroyBuffer();
}