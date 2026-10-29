#pragma once
#include "GLFWConfig.hpp"

#include <cstdint>

#include "UniformData.hpp"

class Pipeline;

//One draw call, described as plain data.
struct DrawItem {

	//Owned by the pipeline cache.
	const Pipeline *pipeline = nullptr;

	//The sets of the tiers above Frame.
	VkDescriptorSet materialSet = VK_NULL_HANDLE;
	VkDescriptorSet objectSet = VK_NULL_HANDLE;

	//Recorded into the command buffer when the item is drawn.
	UniformData::Block pushConstants;
	VkShaderStageFlags pushConstantStages = 0;

	//With an index buffer (of 32-bit indices) count is the number of indices to draw,
	//without one it's the number of vertices.
	VkBuffer vertexBuffer = VK_NULL_HANDLE;
	VkBuffer indexBuffer = VK_NULL_HANDLE;
	uint32_t count = 0;
};