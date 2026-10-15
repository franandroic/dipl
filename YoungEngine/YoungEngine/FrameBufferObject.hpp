#pragma once
#include "GLFWConfig.hpp"

#include <array>

#include "DeviceUtils.hpp"

#include "SwapChain.hpp"
#include "ColorImageObject.hpp"
#include "DepthImageObject.hpp"
#include "TextureImageObject.hpp"

//Encapsulates Vulkan's buffer objects that contain images
//to be rendered to and sampled from.
class FrameBufferObject {

public:

	//Construction
	FrameBufferObject(
		SwapChain &inSwapChain,
		unsigned char *pixels,
		int texWidth, int texHeight, int texChannels
	);
	~FrameBufferObject() = default;

	//Copy and Move constructor
	FrameBufferObject(const FrameBufferObject &) = delete;
	FrameBufferObject &operator=(const FrameBufferObject &) = delete;
	FrameBufferObject(FrameBufferObject &&) = delete;
	FrameBufferObject &operator=(FrameBufferObject &&) = delete;

	//Member variables
	SwapChain &swapChain;

	ColorImageObject CIO;
	DepthImageObject DIO;
	TextureImageObject TIO;

	void recreateAttachments();
};