#include "RenderTarget.hpp"

RenderTarget::RenderTarget(SwapChain &inSwapChain)
	: swapChain(inSwapChain),
	CIO(inSwapChain.device, inSwapChain.swapChainImageFormat),
	DIO(inSwapChain.device, DeviceUtils::findDepthFormat(inSwapChain.device.physical))
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
