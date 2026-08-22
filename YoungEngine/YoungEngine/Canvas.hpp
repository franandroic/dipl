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

//Contains objects used to define the look of an object
class Canvas {

public:

	//Constructor
	Canvas(
		SwapChain &inSwapChain,
		unsigned char *pixels,
		int texWidth, int texHeight, int texChannels
	);

	//Copy and Move constructors
	Canvas(const Canvas &) = delete;
	Canvas &operator=(const Canvas &) = delete;
	Canvas(Canvas &&) = delete;
	Canvas &operator=(Canvas &&) = delete;

	//Member variables
	RenderPass renderPass;

	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	Description description;

	FrameBufferObject FBO;

	std::vector<UniformBufferObject> UBOs;
	UniformBufferData UBdata;
	UniformBufferOperator UBop;

private:

	void createDescriptorPool(SwapChain *swapChain);
};