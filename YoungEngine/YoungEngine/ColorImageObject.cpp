#include "ColorImageObject.hpp"

void ColorImageObject::createImage(uint32_t width, uint32_t height) {

	ImageObject::createImage(
		width,
		height,
		1,
		device->msaaSamples,
		colorFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	createImageView(image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}