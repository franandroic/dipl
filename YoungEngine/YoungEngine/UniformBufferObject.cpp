#include "UniformBufferObject.hpp"

void UniformBufferObject::createBuffer(VkPhysicalDevice physicalDevice) {

	BufferObject::createBuffer(physicalDevice);

	vkMapMemory(device->device, bufferMemory, 0, size, 0, &bufferMapped);
}

void UniformBufferObject::updateBuffer() {

	op->operate(data);

	//TODO: Look into push constants
	memcpy(bufferMapped, data, sizeof(*data));
}