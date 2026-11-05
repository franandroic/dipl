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

	enum class BlendMode {
		Opaque,
		Alpha
	};

	struct RasterState {

		VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;

		bool depthTest = true;
		bool depthWrite = true;

		BlendMode blend = BlendMode::Opaque;

		bool operator==(const RasterState &other) const;
	};

	enum class VertexInput {
		Mesh,
		None
	};

	//The tiers the descriptors are split into, from the least to the most frequently changing.
	enum DescriptorTier : uint32_t {
		Frame = 0,		//shared by everything drawn in a frame (the camera)
		Material = 1,	//what an object looks like (the texture)
		Object = 2,		//data of a single object that goes through a uniform buffer
		Count = 3
	};

	//The Frame tier set is the same for every pipeline, and it's filled by the renderer rather than by
	//whatever is drawn, so it isn't part of a description. Both the pipelines (for their layouts) and
	//the renderer (for its sets) take it from here, so the two can't disagree.
	//Binding 0 is the camera's uniform block (Camera::UniformBlock).
	static constexpr uint32_t FRAME_CAMERA_BINDING = 0;
	static std::vector<VkDescriptorSetLayoutBinding> frameBindings();

	//Member variables
	ShaderPair shaders;
	RenderingFormats formats;
	RasterState raster;
	VertexInput vertexInput = VertexInput::Mesh;

	VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	std::vector<VkDescriptorSetLayoutBinding> materialBindings;
	std::vector<VkDescriptorSetLayoutBinding> objectBindings;

	VkPushConstantRange pushConstantRange{};

	bool operator==(const PipelineDescription &other) const;
};