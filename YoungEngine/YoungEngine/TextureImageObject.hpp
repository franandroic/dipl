#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cmath>

#include "ImageUtils.hpp"
#include "CommandUtils.hpp"
#include "ImageObject.hpp"
#include "Device.hpp"
#include "BufferObject.hpp"

class TextureImageObject : public ImageObject {

public:

	TextureImageObject() = default;

	TextureImageObject(Device *inDevice) : ImageObject(inDevice) {}

	uint32_t mipLevels;

	VkSampler sampler;

	void createImage(VkCommandPool commandPool, unsigned char *pixels, int width, int height, int channels);

	void createTextureSampler();

private:

	void generateMipmaps(
		VkCommandPool commandPool,
		VkFormat imageFormat,
		int32_t texWidth,
		int32_t texHeight,
		uint32_t mipLevels
	);

};