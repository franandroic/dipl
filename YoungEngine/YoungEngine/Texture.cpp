#include "Texture.hpp"

Texture::Texture(
	unsigned char *pixels,
	int inWidth, int inHeight, int inChannels
) : width(inWidth),
	height(inHeight),
	channels(inChannels),
	isRenderTarget(false)
{
	TIO.createImage(pixels, inWidth, inHeight, inChannels);
	TIO.createTextureSampler();
}

Texture::Texture(uint32_t inWidth, uint32_t inHeight, VkFormat inFormat)
  : width(static_cast<int>(inWidth)),
	height(static_cast<int>(inHeight)),
	channels(0),
	isRenderTarget(true)
{
	TIO.createRenderTargetImage(inWidth, inHeight, inFormat);
	TIO.createRenderTargetTextureSampler();
}

void Texture::recordBeginRendering(VkCommandBuffer commandBuffer, const DepthImageObject *depthImage) const {

	if (!isRenderTarget) {
		throw std::logic_error("Only a render target texture can be rendered into!");
	}

	//The contents are rendered anew every frame, so the old ones are discarded (the old layout is undefined).
	//The barrier still has to wait for the fragment shaders that sampled the texture before (in the previous
	//frames, which may still be running), so they're done reading before it's cleared.
	ImageUtility::recordImageLayoutTransition(
		commandBuffer,
		TIO.image,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	);

	//Cleared to transparent black, so whatever no pass draws into reads as "nothing" (e.g. outside of a mask)
	VkRenderingAttachmentInfo colorAttachment{};
	colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	colorAttachment.imageView = TIO.imageView;
	colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.clearValue.color = { { 0.0f, 0.0f, 0.0f, 0.0f } };

	//The depth image only serves this rendering, so it isn't stored
	VkRenderingAttachmentInfo depthAttachment{};

	if (depthImage) {

		VkImageAspectFlags depthAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (ImageUtility::hasStencilComponent(depthImage->depthFormat)) {
			depthAspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}

		ImageUtility::recordImageLayoutTransition(
			commandBuffer,
			depthImage->image,
			depthAspect,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
		);

		depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		depthAttachment.imageView = depthImage->imageView;
		depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.clearValue.depthStencil = { 1.0f, 0 };
	}

	VkRenderingInfo renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
	renderingInfo.renderArea.offset = { 0, 0 };
	renderingInfo.renderArea.extent = getExtent();
	renderingInfo.layerCount = 1;
	renderingInfo.viewMask = 0;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachment;
	renderingInfo.pDepthAttachment = depthImage ? &depthAttachment : nullptr;
	renderingInfo.pStencilAttachment = nullptr;

	vkCmdBeginRendering(commandBuffer, &renderingInfo);
}

void Texture::recordEndRendering(VkCommandBuffer commandBuffer) const {

	if (!isRenderTarget) {
		throw std::logic_error("Only a render target texture can be rendered into!");
	}

	vkCmdEndRendering(commandBuffer);

	ImageUtility::recordImageLayoutTransition(
		commandBuffer,
		TIO.image,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT
	);
}