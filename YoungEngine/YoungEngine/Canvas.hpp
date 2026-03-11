#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

#include "DeviceData.hpp"

#include "SwapChain.hpp"
#include "RenderPass.hpp"
#include "Description.hpp"
#include "FrameBufferObject.hpp"
#include "UniformBufferObject.hpp"
#include "UniformBufferData.hpp"
#include "UniformBufferOperator.hpp"

class Canvas {

public:

	Canvas() = default;

	Canvas(SwapChain *inSwapChain, unsigned char *pixels, int texWidth, int texHeight, int texChannels);

	RenderPass renderPass;

	VkDescriptorPool descriptorPool;
	Description description;

	FrameBufferObject FBO;

	std::vector<UniformBufferObject> UBOs;
	UniformBufferData UBdata;
	UniformBufferOperator UBop;

private:

	void createDescriptorPool(SwapChain *swapChain);
};