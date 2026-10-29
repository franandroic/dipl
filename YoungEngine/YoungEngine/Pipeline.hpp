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
	//setBindings[N] is the list of bindings of set N in the shaders (see DescriptorTier). A set the shaders
	//don't use is an empty list.
	Pipeline(Device &inDevice, const PipelineDescription &inDescription);
	~Pipeline();

	//Copy and Move constructors
	Pipeline(const Pipeline &) = delete;
	Pipeline &operator=(const Pipeline &) = delete;
	Pipeline(Pipeline &&) = delete;
	Pipeline &operator=(Pipeline &&) = delete;

	//Member variables
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;

	//The description of set N. Throws std::out_of_range if the pipeline has no such set.
	const PipelineDescription &getDescription() const { return description; }

private:

	Device &device;
	PipelineDescription description;
	std::vector<VkDescriptorSetLayout> setLayouts;

	VkDescriptorSetLayout createSetLayout(const std::vector<VkDescriptorSetLayoutBinding> &bindings);
	VkShaderModule createShaderModule(const std::vector<char> &code);
	std::vector<char> readFile(const std::string &filename);
	void destroy();
};