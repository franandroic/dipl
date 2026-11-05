#pragma once
#include "GLFWConfig.hpp"

#include <cmath>

#include "ImageUtility.hpp"
#include "CommandUtility.hpp"
#include "ImageObject.hpp"
#include "Device.hpp"
#include "BufferObject.hpp"

//Image object used to store texture data
class TextureImageObject : public ImageObject {

public:

	//Construction
	TextureImageObject() = default;
	~TextureImageObject() override { vkDestroySampler(Device::active->logical, sampler, nullptr); }

	//Member variables
	uint32_t mipLevels = 0;
	VkSampler sampler = VK_NULL_HANDLE;

	//Used to create a texture as a loaded asset
	void createImage(unsigned char *pixels, int width, int height, int channels);
	void createTextureSampler();

	//Used to create a texture as a part of a render pass
	void createRenderTargetImage(uint32_t width, uint32_t height, VkFormat format);
	void createRenderTargetTextureSampler();

private:

	void generateMipmaps(
		VkFormat imageFormat,
		int32_t texWidth,
		int32_t texHeight,
		uint32_t mipLevels
	);

};