#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "ImageUtils.hpp"
#include "Device.hpp"
#include "ImageObject.hpp"

//Image object used to store depth data
class DepthImageObject : public ImageObject {

public:

	//Construction
	DepthImageObject(Device &inDevice, VkFormat inFormat) : ImageObject(inDevice), depthFormat(inFormat) {}

	//Member variables
	const VkFormat depthFormat;

	void createImage(VkCommandPool commandPool, uint32_t width, uint32_t height);
};