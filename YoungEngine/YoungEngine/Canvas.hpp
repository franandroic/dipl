#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <memory>

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

	//Construction
	Canvas(
		SwapChain &inSwapChain,
		unsigned char *pixels,
		int texWidth, int texHeight, int texChannels
	);
	~Canvas();

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

	std::vector<std::unique_ptr<UniformBufferObject>> UBOs;
	UniformBufferData UBdata;
	UniformBufferOperator UBop;

private:

	Device &device;

	void createDescriptorPool(SwapChain &swapChain);
};