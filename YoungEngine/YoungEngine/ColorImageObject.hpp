#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Device.hpp"
#include "ImageObject.hpp"

class ColorImageObject : public ImageObject {

public:

	ColorImageObject() = default;

	ColorImageObject(Device *inDevice, VkFormat inFormat) : ImageObject(inDevice) { colorFormat = inFormat; }

	VkFormat colorFormat;

	void createImage(VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, VkSampleCountFlagBits msaaSamples);
};