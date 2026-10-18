#pragma once
#include "GLFWConfig.hpp"

#include <fstream>
#include <vector>

#include "Device.hpp"
#include "Vertex.hpp"
#include "Description.hpp"
#include "RenderTarget.hpp"

//Contains definition of the graphics pipeline and handles shader code.
//The attachment formats the pipeline renders to are baked into it, so it's created from the
//RenderTarget it will be used with. It doesn't own it: all of the pipelines of a frame share
//the render target of the Renderer, and only the formats are read at construction.
class Pipeline {

public:

	//Construction
	Pipeline(
		Device &inDevice,
		const RenderTarget &renderTarget,
		Description &description,
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

private:

	Device &device;

	//Creating a shader module from SPIR-V code
	VkShaderModule createShaderModule(const std::vector<char> &code);
	
	//Reading shader code compiled to SPIR-V
	std::vector<char> readFile(const std::string &filename);
};