#pragma once
#include "GLFWConfig.hpp"

#include "Device.hpp"
#include "ImageObject.hpp"

//Image object used to store depth data
class DepthImageObject : public ImageObject {

public:

	//Construction
	explicit DepthImageObject(VkFormat inFormat) : depthFormat(inFormat) {}

	//Member variables
	const VkFormat depthFormat;

	void createImage(uint32_t width, uint32_t height, VkSampleCountFlagBits samples = Device::active->msaaSamples);
};