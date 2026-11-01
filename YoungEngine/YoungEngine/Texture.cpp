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