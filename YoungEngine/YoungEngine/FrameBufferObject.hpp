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

//Encapsulates Vulkan's buffer objects that contain images
//to be rendered to and sampled from.
class FrameBufferObject {

public:

	//Construction
	FrameBufferObject(
		SwapChain &inSwapChain,
		RenderPass &renderPass,
		unsigned char *pixels,
		int texWidth, int texHeight, int texChannels
	);
	~FrameBufferObject();

	//Copy and Move constructor
	FrameBufferObject(const FrameBufferObject &) = delete;
	FrameBufferObject &operator=(const FrameBufferObject &) = delete;
	FrameBufferObject(FrameBufferObject &&) = delete;
	FrameBufferObject &operator=(FrameBufferObject &&) = delete;

	//Member variables
	std::vector<VkFramebuffer> framebuffers;
	
	SwapChain &swapChain;

	ColorImageObject CIO;
	DepthImageObject DIO;
	TextureImageObject TIO;

	void createFramebuffers(RenderPass &renderPass);
	void recreateFramebuffers(RenderPass &renderPass);
};