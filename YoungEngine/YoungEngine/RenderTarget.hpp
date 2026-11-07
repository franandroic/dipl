#pragma once
#include "GLFWConfig.hpp"

#include "ImageUtility.hpp"

#include "DepthImageObject.hpp"
#include "PipelineDescription.hpp"

//Contains images that renderer draws into (apart from swap chain images).
//the multisampled color image (resolved into the swap chain image) and the depth image.
class RenderTarget {

public:

	//Construction
	RenderTarget() = default;
	virtual ~RenderTarget() = default;

	//Copy and Move constructor
	RenderTarget(const RenderTarget &) = delete;
	RenderTarget &operator=(const RenderTarget &) = delete;
	RenderTarget(RenderTarget &&) = delete;
	RenderTarget &operator=(RenderTarget &&) = delete;

	//Parameters of pipeline attachments, for the pipeline that renders into this
	virtual PipelineDescription::RenderingFormats getRenderingFormats() const = 0;
	virtual VkExtent2D getExtent() const = 0;

	//Records barriers needed at start and end of rendering commands
	virtual void recordBegin(VkCommandBuffer commandBuffer) const = 0;
	virtual void recordEnd(VkCommandBuffer commandBuffer) const = 0;

protected:

	static void recordDepthBarrier(VkCommandBuffer commandBuffer, const DepthImageObject &depthImage);
	static VkRenderingAttachmentInfo makeDepthAttachment(const DepthImageObject &depthImage);
};