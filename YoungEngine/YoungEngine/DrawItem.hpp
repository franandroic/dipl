#pragma once
#include "GLFWConfig.hpp"

#include <cstdint>

#include "UniformData.hpp"

class Pipeline;
class Material;

//One draw call, described as plain data.
struct DrawItem {

	//Owned by the pipeline cache.
	const Pipeline *pipeline = nullptr;

	//The material and the pass of it the item draws, which also say where it's drawn into.
	//Null for something drawn without a material (always into the screen).
	const Material *material = nullptr;
	uint32_t pass = 0;

	//The sets of the tiers above Frame.
	VkDescriptorSet materialSet = VK_NULL_HANDLE;
	VkDescriptorSet objectSet = VK_NULL_HANDLE;

	//Recorded into the command buffer when the item is drawn.
	UniformData::Block pushConstants;
	VkShaderStageFlags pushConstantStages = 0;

	//With an index buffer (of 32-bit indices) count is the number of indices to draw,
	//without one it's the number of vertices. Without a vertex buffer either (a fullscreen pass),
	//nothing is bound and the vertex shader makes the vertices out of gl_VertexIndex.
	VkBuffer vertexBuffer = VK_NULL_HANDLE;
	VkBuffer indexBuffer = VK_NULL_HANDLE;
	uint32_t count = 0;
};