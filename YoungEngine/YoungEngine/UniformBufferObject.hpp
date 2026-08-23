#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <algorithm>

#include "BufferObject.hpp"
#include "UniformBufferData.hpp"
#include "UniformBufferOperator.hpp"

//Buffer object that defines uniform data to be passed to a shader.
class UniformBufferObject : public BufferObject {

	//TODO: Two strategy (I think) patterns, the idea is to switch out the data type and the operator
	//for different UBOs

public:

	//Constructor
	UniformBufferObject(
		Device &inDevice,
		UniformBufferData &inData,
		UniformBufferOperator &inOp,
		VkBufferUsageFlags inUsage,
		VkMemoryPropertyFlags inProperties
	) : BufferObject(inDevice, sizeof(inData), inUsage, inProperties),
		data(inData),
		op(inOp) {}

	//Member variables
	void *bufferMapped = nullptr;

	void createBuffer();
	void updateBuffer();

private:

	UniformBufferData &data;
	UniformBufferOperator &op;
};