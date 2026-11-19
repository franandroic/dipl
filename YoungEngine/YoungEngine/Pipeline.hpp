#pragma once
#include "GLFWConfig.hpp"

#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "Device.hpp"
#include "Vertex.hpp"
#include "PipelineDescription.hpp"
#include "ShaderCompiler.hpp"

//Contains definition of the graphics pipeline and handles shader code.
class Pipeline {

public:

	//Construction
	explicit Pipeline(const PipelineDescription &inDescription);
	~Pipeline();

	//Copy and Move constructors
	explicit Pipeline(const Pipeline &) = delete;
	Pipeline &operator=(const Pipeline &) = delete;
	Pipeline(Pipeline &&) = delete;
	Pipeline &operator=(Pipeline &&) = delete;

	//Member variables
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;

	//The description of set N.
	const PipelineDescription &getDescription() const { return description; }

	//Recreates the graphicsPipeline with current shaders.
	void rebuild();

private:

	PipelineDescription description;
	std::vector<VkDescriptorSetLayout> setLayouts;

	static VkPipelineColorBlendAttachmentState makeBlendState(PipelineDescription::BlendMode mode);
	VkDescriptorSetLayout createSetLayout(const std::vector<VkDescriptorSetLayoutBinding> &bindings);
	VkShaderModule createShaderModule(const std::vector<uint32_t> &code) const;

	void createLayouts();
	VkPipeline createGraphicsPipeline() const;

	void destroy();
};