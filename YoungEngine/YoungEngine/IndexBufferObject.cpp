#include "IndexBufferObject.hpp"

void IndexBufferObject::createBuffer(VkPhysicalDevice physicalDevice, VkCommandPool commandPool) {

	//Same as the vertex memory, but for the index buffer.

	BufferObject stagingBufferObject(
		device,
		size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	stagingBufferObject.createBuffer(physicalDevice);

	void *data;
	vkMapMemory(device->device, stagingBufferObject.bufferMemory, 0, size, 0, &data);
	memcpy(data, (*indices).data(), (size_t)size);
	vkUnmapMemory(device->device, stagingBufferObject.bufferMemory);

	BufferObject::createBuffer(physicalDevice);

	copyBuffer(stagingBufferObject.buffer, buffer, size, commandPool);

	vkDestroyBuffer(device->device, stagingBufferObject.buffer, nullptr);
	vkFreeMemory(device->device, stagingBufferObject.bufferMemory, nullptr);
}