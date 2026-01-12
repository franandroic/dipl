#include "ColorImageObject.hpp"

void ColorImageObject::createImage(VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, VkSampleCountFlagBits msaaSamples) {

	ImageObject::createImage(
		physicalDevice,
		width,
		height,
		1,
		msaaSamples,
		colorFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
}