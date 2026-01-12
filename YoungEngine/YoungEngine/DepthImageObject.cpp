#include "DepthImageObject.hpp"

void DepthImageObject::createImage(VkPhysicalDevice physicalDevice, VkCommandPool commandPool, uint32_t width, uint32_t height, VkSampleCountFlagBits msaaSamples) {

	ImageObject::createImage(
		physicalDevice,
		width,
		height,
		1,
		msaaSamples,
		depthFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	ImageUtils::transitionImageLayout(device->device, commandPool, device->graphicsQueue, image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}