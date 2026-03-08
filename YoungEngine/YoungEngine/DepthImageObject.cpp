#include "DepthImageObject.hpp"

void DepthImageObject::createImage(VkCommandPool commandPool, uint32_t width, uint32_t height) {

	ImageObject::createImage(
		width,
		height,
		1,
		device->msaaSamples,
		depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	ImageUtils::transitionImageLayout(
		device->logical,
		commandPool,
		device->graphicsQueue,
		image,
		depthFormat,
		VK_IMAGE_ASPECT_DEPTH_BIT,
		1,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	);

	createImageView(image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
}