#include "UniformBufferObject.hpp"

void UniformBufferObject::createBuffer() {

	//Creating and mapping a uniform buffer for each frame (imageView) we want to be able
	//to draw to before presenting.

	BufferObject::createBuffer();

	vkMapMemory(device->logical, bufferMemory, 0, size, 0, &bufferMapped);
}

void UniformBufferObject::updateBuffer() {

	op->operate(data);

	//TODO: Look into push constants
	memcpy(bufferMapped, data, sizeof(*data));
}