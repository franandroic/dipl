#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>

#include "DeviceUtils.hpp"

#include "SwapChain.hpp"
#include "ColorImageObject.hpp"
#include "DepthImageObject.hpp"
#include "TextureImageObject.hpp"
#include "RenderPass.hpp"

class FrameBufferObject {

public:

	FrameBufferObject() = default;

	FrameBufferObject(SwapChain *inSwapChain, RenderPass *renderPass, unsigned char *pixels, int texWidth, int texHeight, int texChannels);

	std::vector<VkFramebuffer> framebuffers;

	SwapChain *swapChain;

	ColorImageObject CIO;
	DepthImageObject DIO;
	TextureImageObject TIO;

	void createFramebuffers(RenderPass *renderPass);

	void recreateFramebuffers(RenderPass *renderPass);
};