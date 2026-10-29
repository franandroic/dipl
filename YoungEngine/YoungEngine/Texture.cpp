#include "Texture.hpp"

Texture::Texture(
	Device &inDevice,
	unsigned char *pixels,
	int inWidth, int inHeight, int inChannels
) : width(inWidth),
	height(inHeight),
	channels(inChannels),
	TIO(inDevice)
{
	TIO.createImage(inDevice.commandPool, pixels, inWidth, inHeight, inChannels);
	TIO.createTextureSampler();
}