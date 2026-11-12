#pragma once
#include "GLFWConfig.hpp"
#include "GLMConfig.hpp"

#include <cstddef>

#include "UniformData.hpp"
#include "LightSource.hpp"

class SimpleLitUD : public UniformData {

public:

	struct PushConstants {
		glm::mat4 model{ 1.0f };
	};

	struct LightBlock {
		alignas(4) float intensity = 0.0f;
		alignas(16) glm::vec3 position{ 0.0f };
	};

	explicit SimpleLitUD(const LightSource &light, const glm::vec3 &inPosition = glm::vec3(0.0f)) : position(inPosition) {
		pushConstants.model = glm::translate(glm::mat4(1.0f), position);
		setLight(light);
	}

	//Member variables
	glm::vec3 position;
	PushConstants pushConstants;
	LightBlock lightBlock;

	void setLight(const LightSource &light) {
		lightBlock.position = light.position;
		lightBlock.intensity = light.intensity;
	}

	Block bufferBlock() const override { return { &lightBlock, sizeof(LightBlock) }; }
	Block pushConstantBlock() const override { return { &pushConstants, sizeof(PushConstants) }; }

	VkShaderStageFlags bufferStages() const override { return VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT; }
	VkShaderStageFlags pushConstantStages() const override { return VK_SHADER_STAGE_VERTEX_BIT; }
};