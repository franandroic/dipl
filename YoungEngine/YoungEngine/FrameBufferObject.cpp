#include "FrameBufferObject.hpp"

FrameBufferObject::FrameBufferObject(
	SwapChain &inSwapChain,
	unsigned char *pixels,
	int texWidth, int texHeight, int texChannels
) : swapChain(inSwapChain),
	CIO(inSwapChain.device, inSwapChain.swapChainImageFormat),
	DIO(inSwapChain.device, DeviceUtils::findDepthFormat(inSwapChain.device.physical)),
	TIO(inSwapChain.device)
{
	CIO.createImage(swapChain.swapChainExtent.width, swapChain.swapChainExtent.height);
	DIO.createImage(swapChain.swapChainExtent.width, swapChain.swapChainExtent.height);
	TIO.createImage(swapChain.device.commandPool, pixels, texWidth, texHeight, texChannels);
	TIO.createTextureSampler();
}

void FrameBufferObject::recreateAttachments() {

	CIO.destroyImage();
	DIO.destroyImage();

	CIO.createImage(swapChain.swapChainExtent.width, swapChain.swapChainExtent.height);
	DIO.createImage(swapChain.swapChainExtent.width, swapChain.swapChainExtent.height);
}