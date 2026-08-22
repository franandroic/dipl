#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <fstream>
#include <vector>

#include "Device.hpp"
#include "Vertex.hpp"
#include "RenderPass.hpp"
#include "Description.hpp"

//Contains definition of the graphics pipeline and handles shader code
class Pipeline {

public:

	//Constructor
	Pipeline(Device &inDevice, RenderPass &renderPass, Description &description);

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