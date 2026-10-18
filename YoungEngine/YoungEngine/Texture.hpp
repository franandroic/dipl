#pragma once
#include "GLFWConfig.hpp"

#include "Device.hpp"
#include "TextureImageObject.hpp"

//Everything that belongs to one texture instance: the image (with its mip chain and view),
//the sampler used to read it, and its dimensions.
//A texture is sampled from and can be shared by multiple materials.
class Texture {

public:

	//Construction
	Texture(
		Device &inDevice,
		unsigned char *pixels,
		int inWidth, int inHeight, int inChannels
	);
	~Texture() = default;

	//Copy and Move constructors
	Texture(const Texture &) = delete;
	Texture &operator=(const Texture &) = delete;
	Texture(Texture &&) = delete;
	Texture &operator=(Texture &&) = delete;

	//Member variables
	const int width;
	const int height;
	const int channels;

	VkImageView getImageView() const { return TIO.imageView; }
	VkSampler getSampler() const { return TIO.sampler; }
	uint32_t getMipLevels() const { return TIO.mipLevels; }

private:
	
	TextureImageObject TIO;
};