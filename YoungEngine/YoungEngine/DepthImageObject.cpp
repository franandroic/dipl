#include "DepthImageObject.hpp"

void DepthImageObject::createImage(uint32_t width, uint32_t height, VkSampleCountFlagBits samples) {

	ImageObject::createImage(
		width,
		height,
		1,
		samples,
		depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	createImageView(image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
}