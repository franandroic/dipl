#pragma once
#include "GLFWConfig.hpp"

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