#pragma once
#include "GLFWConfig.hpp"

#include "DeviceUtility.hpp"
#include "ImageUtility.hpp"

#include "SwapChain.hpp"
#include "ColorImageObject.hpp"
#include "DepthImageObject.hpp"
#include "RenderTarget.hpp"

class ScreenRenderTarget : public RenderTarget {

public:

	//Construction
	ScreenRenderTarget();
	~ScreenRenderTarget() override = default;

	PipelineDescription::RenderingFormats getRenderingFormats() const override;
	VkExtent2D getExtent() const override { return SwapChain::active->swapChainExtent; }

	//The swap chain image the next rendering is resolved into (and transitioned for presentation)
	void setSwapChainImage(uint32_t inImageIndex) { imageIndex = inImageIndex; }

	void recordBegin(VkCommandBuffer commandBuffer) const override;
	void recordEnd(VkCommandBuffer commandBuffer) const override;

	void recreateAttachments();

private:

	ColorImageObject CIO;
	DepthImageObject DIO;

	uint32_t imageIndex = 0;
};