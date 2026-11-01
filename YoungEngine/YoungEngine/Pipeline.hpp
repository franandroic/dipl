#pragma once
#include "GLFWConfig.hpp"

#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "Device.hpp"
#include "Vertex.hpp"
#include "PipelineDescription.hpp"

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

	//The description of set N. Throws std::out_of_range if the pipeline has no such set.
	const PipelineDescription &getDescription() const { return description; }

private:

	PipelineDescription description;
	std::vector<VkDescriptorSetLayout> setLayouts;

	VkDescriptorSetLayout createSetLayout(const std::vector<VkDescriptorSetLayoutBinding> &bindings);
	VkShaderModule createShaderModule(const std::vector<char> &code);
	std::vector<char> readFile(const std::string &filename);

	void destroy();
};