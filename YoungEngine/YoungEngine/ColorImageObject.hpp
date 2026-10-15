#pragma once
#include "GLFWConfig.hpp"

#include "Device.hpp"
#include "ImageObject.hpp"

//Image object used to store color data
class ColorImageObject : public ImageObject {

public:

	//Construction
	ColorImageObject(Device &inDevice, VkFormat inFormat) : ImageObject(inDevice), colorFormat(inFormat) {}

	//Member variables
	const VkFormat colorFormat;

	void createImage(uint32_t width, uint32_t height);
};