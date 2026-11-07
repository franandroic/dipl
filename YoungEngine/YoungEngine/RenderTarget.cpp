#include "RenderTarget.hpp"

void RenderTarget::recordDepthBarrier(VkCommandBuffer commandBuffer, const DepthImageObject &depthImage) {

	//Records the transition of the depth image to the depth attachment layout, discarding its old contents
	//The depth image may be shared between the frames in flight, so the barrier waits for the depth tests
	//of the previous frames before the image is cleared again.

	VkImageAspectFlags depthAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	if (ImageUtility::hasStencilComponent(depthImage.depthFormat)) {
		depthAspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	ImageUtility::recordImageLayoutTransition(
		commandBuffer,
		depthImage.image,
		depthAspect,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
	);
}

VkRenderingAttachmentInfo RenderTarget::makeDepthAttachment(const DepthImageObject &depthImage) {

	//Describes the depth image as an attachment that's cleared and not stored

	VkRenderingAttachmentInfo depthAttachment{};
	depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
	depthAttachment.imageView = depthImage.imageView;
	depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depthAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.clearValue.depthStencil = { 1.0f, 0 };

	return depthAttachment;
}
