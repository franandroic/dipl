#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <fstream>
#include <vector>

#include "Device.hpp"
#include "Vertex.hpp"
#include "RenderPass.hpp"
#include "Description.hpp"

class Pipeline {

public:

	Pipeline() = default;

	Pipeline(Device *inDevice, RenderPass *renderPass, Description *description);

	VkPipelineLayout pipelineLayout;

	VkPipeline graphicsPipeline;

private:

	Device *device;

	VkShaderModule createShaderModule(const std::vector<char> &code);

	//FUNCTION TO READ SHADER CODE COMPILED TO SPIR-V
	std::vector<char> readFile(const std::string &filename);
};