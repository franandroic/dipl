#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>

#include "DeviceUtils.hpp"

#include "Device.hpp"
#include "SwapChain.hpp"

//Wrapper for Vulkan RenderPass
class RenderPass {

public:

	//Constructor
	RenderPass(SwapChain &swapChain);

	//Copy and Move constructors
	RenderPass(const RenderPass &) = delete;
	RenderPass &operator=(const RenderPass &) = delete;
	RenderPass(RenderPass &&) = delete;
	RenderPass &operator=(RenderPass &&) = delete;

	//Member variables
	VkRenderPass renderPass = VK_NULL_HANDLE;
};