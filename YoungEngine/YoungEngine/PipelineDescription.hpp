#pragma once
#include "GLFWConfig.hpp"

#include <cstdint>
#include <string>
#include <vector>

//Everything that makes one graphics pipeline different from another: the shaders it runs, the layouts of
//the descriptor sets they read, the push constants they use and the attachments it renders to.
class PipelineDescription {

public:

	//The pair of compiled shaders (paths to SPIR-V files) a pipeline runs
	struct ShaderPair {

		std::string vertex;
		std::string fragment;

		bool operator==(const ShaderPair &other) const;
	};

	//The attachments a pipeline renders into.
	struct RenderingFormats {

		//VK_FORMAT_UNDEFINED means there is no such attachment
		VkFormat colorFormat = VK_FORMAT_UNDEFINED;
		VkFormat depthFormat = VK_FORMAT_UNDEFINED;
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;

		bool hasColor() const { return colorFormat != VK_FORMAT_UNDEFINED; }
		bool hasDepth() const { return depthFormat != VK_FORMAT_UNDEFINED; }

		bool operator==(const RenderingFormats &other) const;
	};

	//The tiers the descriptors are split into, from the least to the most frequently changing.
	enum DescriptorTier : uint32_t {
		Frame = 0,		//shared by everything drawn in a frame (the camera)
		Material = 1,	//what an object looks like (the texture)
		Object = 2,		//data of a single object that goes through a uniform buffer
		Count = 3
	};

	//Member variables
	ShaderPair shaders;
	RenderingFormats formats;

	std::vector<VkDescriptorSetLayoutBinding> materialBindings;
	std::vector<VkDescriptorSetLayoutBinding> objectBindings;

	VkPushConstantRange pushConstantRange{};

	bool operator==(const PipelineDescription &other) const;
};