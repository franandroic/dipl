#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "ImageUtils.hpp"
#include "Device.hpp"
#include "ImageObject.hpp"

class DepthImageObject : public ImageObject {

public:

	DepthImageObject() = default;

	DepthImageObject(Device *inDevice, VkFormat inFormat) : ImageObject(inDevice) { depthFormat = inFormat; }

	VkFormat depthFormat;

	void createImage(VkCommandPool commandPool, uint32_t width, uint32_t height);

};