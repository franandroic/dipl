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

	//Construction
	explicit SimpleModelUD(const glm::vec3 &inPosition = glm::vec3(0.0f)) : position(inPosition) {
		pushConstants.model = glm::translate(glm::mat4(1.0f), position);
	}

	//Member variables
	glm::vec3 position;
	PushConstants pushConstants;

	Block pushConstantBlock() const override { return { &pushConstants, sizeof(PushConstants) }; }

	VkShaderStageFlags pushConstantStages() const override { return VK_SHADER_STAGE_VERTEX_BIT; }
};