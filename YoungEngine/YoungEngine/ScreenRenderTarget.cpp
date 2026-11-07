#include "ScreenRenderTarget.hpp"

ScreenRenderTarget::ScreenRenderTarget()
  : CIO(SwapChain::active->swapChainImageFormat),
	DIO(DeviceUtility::findDepthFormat(Device::active->physical))
{
	CIO.createImage(SwapChain::active->swapChainExtent.width, SwapChain::active->swapChainExtent.height);
	DIO.createImage(SwapChain::active->swapChainExtent.width, SwapChain::active->swapChainExtent.height);
}

PipelineDescription::RenderingFormats ScreenRenderTarget::getRenderingFormats() const {

	return { CIO.colorFormat, DIO.depthFormat, Device::active->msaaSamples };
}

void ScreenRenderTarget::recreateAttachments() {

	//Recreates the images to match the current swap chain extent

	CIO.destroyImage();
	DIO.destroyImage();

	CIO.createImage(SwapChain::active->swapChainExtent.width, SwapChain::active->swapChainExtent.height);
	DIO.createImage(SwapChain::active->swapChainExtent.width, SwapChain::active->swapChainExtent.height);
}

void ScreenRenderTarget::recordBegin(VkCommandBuffer commandBuffer) const {

	//Without a render pass, nothing transitions the layouts of the attachments or synchronizes
	//access to them for us. That's why the rendering is bracketed by explicit image barriers.
	//All three images are written from scratch (cleared, or fully resolved into).

	//The swap chain image receives the resolved result
	ImageUtility::recordImageLayoutTransition(
		commandBuffer,
		SwapChain::active->swapChainImages[imageIndex],
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	);

	//The multisampled color image and the depth image are shared between the frames in flight
	ImageUtility::recordImageLayoutTransition(
		commandBuffer,
		CIO.image,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	);

	recordDepthBarrier(commandBuffer, DIO);

	//Describing the attachments directly, instead of through a render pass and a framebuffer.
	//The color attachment is the multisampled image, which is resolved into the swap chain image
	//at the end of rendering. The multisampled contents aren't needed afterwards, so they're not stored.
	VkRenderingAttachmentInfo colorAttachment{};
	colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	colorAttachment.imageView = CIO.imageView;
	colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
	colorAttachment.resolveImageView = SwapChain::active->swapChainImageViews[imageIndex];
	colorAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.clearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

	VkRenderingAttachmentInfo depthAttachment = makeDepthAttachment(DIO);

	VkRenderingInfo renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderingInfo.renderArea.offset = { 0, 0 };
	renderingInfo.renderArea.extent = getExtent();
	renderingInfo.layerCount = 1;
	renderingInfo.viewMask = 0;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachment;
	renderingInfo.pDepthAttachment = &depthAttachment;
	renderingInfo.pStencilAttachment = nullptr;

	vkCmdBeginRendering(commandBuffer, &renderingInfo);
}

void ScreenRenderTarget::recordEnd(VkCommandBuffer commandBuffer) const {

	vkCmdEndRendering(commandBuffer);

	//The resolved swap chain image goes to the layout required for presentation.
	//The render pass used to do this through the finalLayout of the resolve attachment.
	ImageUtility::recordImageLayoutTransition(
		commandBuffer,
		SwapChain::active->swapChainImages[imageIndex],
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0
	);
}
