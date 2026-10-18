#include "UniformBufferObject.hpp"

#include <cstring>
#include <stdexcept>

void UniformBufferObject::createBuffer() {

	//Creating and mapping a uniform buffer for each frame (imageView) we want to be able
	//to draw to before presenting.

	BufferObject::createBuffer();

	vkMapMemory(device.logical, bufferMemory, 0, size, 0, &bufferMapped);
}

void UniformBufferObject::updateBuffer(const void *data, size_t dataSize) {

	//Updating the contents of the memory mapped to the GPU passes the values to the shaders.

	if (dataSize > size) {
		throw std::length_error("Data doesn't fit into the uniform buffer!");
	}

	memcpy(bufferMapped, data, dataSize);
}