#include "TextureRenderTarget.hpp"

TextureRenderTarget::TextureRenderTarget(VkExtent2D inExtent, VkFormat format, bool depth)
	: colorFormat(format),
	texture(inExtent.width, inExtent.height, format) {

	if (depth) {
		depthImage = std::make_unique<DepthImageObject>(DeviceUtility::findDepthFormat(Device::active->physical));
		depthImage->createImage(inExtent.width, inExtent.height, VK_SAMPLE_COUNT_1_BIT);
	}
}

PipelineDescription::RenderingFormats TextureRenderTarget::formatsFor(VkFormat format, bool depth) {

	//The formats a texture render target of the given color format (and depth or not) has.
	//Known without making one, so pipelines can be described before the target exists.

	return {
		format,
		depth ? DeviceUtility::findDepthFormat(Device::active->physical) : VK_FORMAT_UNDEFINED,
		VK_SAMPLE_COUNT_1_BIT
	};
}

PipelineDescription::RenderingFormats TextureRenderTarget::getRenderingFormats() const {

	return formatsFor(colorFormat, depthImage != nullptr);
}

void TextureRenderTarget::recordBegin(VkCommandBuffer commandBuffer) const {

	//The contents are rendered anew every frame, so the old ones are discarded (the old layout is undefined).
	//The barrier still has to wait for the fragment shaders that sampled the texture before (in the previous
	//frames, which may still be running), so they're done reading before it's cleared.
	ImageUtility::recordImageLayoutTransition(
		commandBuffer,
		texture.getImage(),
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	);

	//Cleared to transparent black, so whatever no pass draws into reads as "nothing" (e.g. outside of a mask)
	VkRenderingAttachmentInfo colorAttachment{};
	colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	colorAttachment.imageView = texture.getImageView();
	colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.clearValue.color = { { 0.0f, 0.0f, 0.0f, 0.0f } };

	VkRenderingAttachmentInfo depthAttachment{};
	if (depthImage) {
		recordDepthBarrier(commandBuffer, *depthImage);
		depthAttachment = makeDepthAttachment(*depthImage);
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

void TextureRenderTarget::recordEnd(VkCommandBuffer commandBuffer) const {

	vkCmdEndRendering(commandBuffer);

	//Made ready to be sampled by the fragment shaders of whatever is recorded after it
	ImageUtility::recordImageLayoutTransition(
		commandBuffer,
		texture.getImage(),
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT
	);
}
