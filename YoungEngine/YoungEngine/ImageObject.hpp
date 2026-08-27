#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Device.hpp"

//Abstract class for any object that represents a rendering target
class ImageObject {

public:

	//Construction
	ImageObject(Device &inDevice) : device(inDevice) {}
	virtual ~ImageObject() { destroyImage(); }

	//Copy and Move constructors
	ImageObject(const ImageObject &) = delete;
	ImageObject &operator=(const ImageObject &) = delete;
	ImageObject(ImageObject &&) = delete;
	ImageObject &operator=(ImageObject &&) = delete;

	//Member variables
	VkImage image = VK_NULL_HANDLE;
	VkImageView imageView = VK_NULL_HANDLE;
	VkDeviceMemory imageMemory = VK_NULL_HANDLE;

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
	void destroyImage();

protected:

	Device &device;

	void createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
};