#include "VertexBufferObject.hpp"

void VertexBufferObject::createBuffer(VkCommandPool commandPool) {

	//Creating a vertex buffer, by a two-step process of first creating a CPU-accessible buffer on the GPU,
	//copying the vertex data from the CPU memory to the GPU staging memory, and finally
	//copying from the GPU staging (host-visible) memory to the GPU device-local faster memory.
	//We free the staging memory in the end to avoid having duplicate data.

	BufferObject stagingBufferObject(
		device,
		size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	stagingBufferObject.createBuffer();

	void *data;
	vkMapMemory(device->logical, stagingBufferObject.bufferMemory, 0, size, 0, &data);
	memcpy(data, (*vertices).data(), (size_t)size);
	vkUnmapMemory(device->logical, stagingBufferObject.bufferMemory);

	BufferObject::createBuffer();

	copyBuffer(stagingBufferObject.buffer, buffer, size, commandPool);

	vkDestroyBuffer(device->logical, stagingBufferObject.buffer, nullptr);
	vkFreeMemory(device->logical, stagingBufferObject.bufferMemory, nullptr);
}