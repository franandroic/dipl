#include "Texture.hpp"

Texture::Texture(
	unsigned char *pixels,
	int inWidth, int inHeight, int inChannels
) : width(inWidth),
	height(inHeight),
	channels(inChannels)
{
	TIO.createImage(pixels, inWidth, inHeight, inChannels);
	TIO.createTextureSampler();
}

Texture::Texture(uint32_t inWidth, uint32_t inHeight, VkFormat inFormat)
  : width(static_cast<int>(inWidth)),
	height(static_cast<int>(inHeight)),
	channels(0)
{
	TIO.createRenderTargetImage(inWidth, inHeight, inFormat);
	TIO.createRenderTargetTextureSampler();
}
