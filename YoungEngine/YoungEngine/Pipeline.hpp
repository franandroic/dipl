#pragma once
#include "GLFWConfig.hpp"

#include <fstream>
#include <vector>

#include "Device.hpp"
#include "SwapChain.hpp"
#include "Vertex.hpp"
#include "Description.hpp"
#include "RenderTarget.hpp"

//Contains definition of the graphics pipeline and handles shader code
class Pipeline {

public:

	//Construction
	Pipeline(
		Device &inDevice,
		SwapChain &inSwapChain,
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

	RenderTarget renderTarget;

private:

	Device &device;

	//Creating a shader module from SPIR-V code
	VkShaderModule createShaderModule(const std::vector<char> &code);
	//Reading shader code compiled to SPIR-V
	std::vector<char> readFile(const std::string &filename);
};