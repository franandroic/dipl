#pragma once
#include "GLFWConfig.hpp"
#include "GLMConfig.hpp"

#include "UniformData.hpp"

//Uniform data of a simple object: just a model matrix, passed to the vertex shader as a push constant.
class SimpleModelUD : public UniformData {

public:

	//Matches the push constant block in the vertex shader
	struct PushConstants {
		glm::mat4 model{ 1.0f };
	};

	PushConstants pushConstants;

	Block pushConstantBlock() const override { return { &pushConstants, sizeof(PushConstants) }; }

	VkShaderStageFlags pushConstantStages() const override { return VK_SHADER_STAGE_VERTEX_BIT; }
};