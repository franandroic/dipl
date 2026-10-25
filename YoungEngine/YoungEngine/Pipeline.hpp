#pragma once
#include "GLFWConfig.hpp"

#include <fstream>
#include <memory>
#include <vector>

#include "Device.hpp"
#include "Vertex.hpp"
#include "Description.hpp"
#include "RenderTarget.hpp"

//Contains definition of the graphics pipeline and handles shader code.
//The attachment formats the pipeline renders to are baked into it, so it's created from the
//RenderTarget it will be used with. It doesn't own it: all of the pipelines of a frame share
//the render target of the Renderer, and only the formats are read at construction.
//
//It also owns the Descriptions (descriptor set layouts) of the sets its shaders use. It knows nothing about
//the data behind them: the shape of each set is given to it from the outside, as a list of bindings.
//The objects that own the data then allocate their descriptor sets from the Description of the pipeline.
class Pipeline {

public:

	//Construction
	//setBindings[N] is the list of bindings of set N in the shaders (see DescriptorTier). A set the shaders
	//don't use is an empty list.
	Pipeline(
		Device &inDevice,
		const RenderTarget &renderTarget,
		const std::vector<std::vector<VkDescriptorSetLayoutBinding>> &setBindings,
		const VkPushConstantRange &pushConstantRange
	);
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
	const Description &getDescription(size_t set) const { return *descriptions.at(set); }

private:

	Device &device;
	
	std::vector<std::unique_ptr<Description>> descriptions;

	//Creating a shader module from SPIR-V code
	VkShaderModule createShaderModule(const std::vector<char> &code);
	
	//Reading shader code compiled to SPIR-V
	std::vector<char> readFile(const std::string &filename);
};