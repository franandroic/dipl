#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Device.hpp"

class ImageObject {

public:

	ImageObject() = default;

	ImageObject(Device *inDevice);

	VkImage image;

	VkImageView imageView;

	VkDeviceMemory imageMemory;

	void createImage(
		uint32_t width,
		uint32_t height,
		uint32_t mipLevels,
		VkSampleCountFlagBits numSamples,
		VkFormat format,
		VkImageTiling tiling,
		VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties
	);

protected:

	Device *device;

	void createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
};