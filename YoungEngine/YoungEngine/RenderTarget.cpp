#include "RenderTarget.hpp"

RenderTarget::RenderTarget(SwapChain &inSwapChain)
	: swapChain(inSwapChain),
	CIO(*Device::active, inSwapChain.swapChainImageFormat),
	DIO(*Device::active, DeviceUtils::findDepthFormat(Device::active->physical))
{
	CIO.createImage(swapChain.swapChainExtent.width, swapChain.swapChainExtent.height);
	DIO.createImage(swapChain.swapChainExtent.width, swapChain.swapChainExtent.height);
}

void RenderTarget::recreateAttachments() {

	CIO.destroyImage();
	DIO.destroyImage();

	CIO.createImage(swapChain.swapChainExtent.width, swapChain.swapChainExtent.height);
	DIO.createImage(swapChain.swapChainExtent.width, swapChain.swapChainExtent.height);
}
