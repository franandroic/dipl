#pragma once
#include "GLFWConfig.hpp"

#include "DeviceUtility.hpp"

#include "SwapChain.hpp"
#include "ColorImageObject.hpp"
#include "DepthImageObject.hpp"
#include "PipelineDescription.hpp"

//Contains images that renderer draws into (apart from swap chain images).
//the multisampled color image (resolved into the swap chain image) and the depth image.
class RenderTarget {

public:

	//Construction
	RenderTarget();
	~RenderTarget() = default;

	//Copy and Move constructor
	RenderTarget(const RenderTarget &) = delete;
	RenderTarget &operator=(const RenderTarget &) = delete;
	RenderTarget(RenderTarget &&) = delete;
	RenderTarget &operator=(RenderTarget &&) = delete;

	//Member variables
	ColorImageObject CIO;
	DepthImageObject DIO;

	//Formats the attachments are created with, needed to create a pipeline that renders to them
	VkFormat colorFormat() const { return CIO.colorFormat; }
	VkFormat depthFormat() const { return DIO.depthFormat; }

	PipelineDescription::RenderingFormats renderingFormats() const {
		return { CIO.colorFormat, DIO.depthFormat, Device::active->msaaSamples };
	}

	//Recreates the images to match the current swap chain extent
	void recreateAttachments();
};