#pragma once
#include "GLFWConfig.hpp"

#include "Device.hpp"
#include "TextureImageObject.hpp"
#include "DepthImageObject.hpp"

//Everything that belongs to one texture instance: the image (with its mip chain and view),
//the sampler used to read it, its dimensions and the descriptor set the shaders read it through.
//A texture is sampled from and can be shared by multiple materials.
class Texture {

public:

	//Construction
	Texture(
		unsigned char *pixels,
		int inWidth, int inHeight, int inChannels
	);

	Texture(uint32_t inWidth, uint32_t inHeight, VkFormat inFormat);

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
	const bool isRenderTarget;

	VkImageView getImageView() const { return TIO.imageView; }
	VkSampler getSampler() const { return TIO.sampler; }
	uint32_t getMipLevels() const { return TIO.mipLevels; }
	VkExtent2D getExtent() const { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }

	void recordBeginRendering(VkCommandBuffer commandBuffer, const DepthImageObject *depthImage = nullptr) const;
	void recordEndRendering(VkCommandBuffer commandBuffer) const;

private:

	TextureImageObject TIO;
};