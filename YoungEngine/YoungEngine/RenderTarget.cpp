#include "RenderTarget.hpp"

RenderTarget::RenderTarget()
	: CIO(SwapChain::active->swapChainImageFormat),
	DIO(DeviceUtility::findDepthFormat(Device::active->physical))
{
	CIO.createImage(SwapChain::active->swapChainExtent.width, SwapChain::active->swapChainExtent.height);
	DIO.createImage(SwapChain::active->swapChainExtent.width, SwapChain::active->swapChainExtent.height);
}

void RenderTarget::recreateAttachments() {

	CIO.destroyImage();
	DIO.destroyImage();

	CIO.createImage(SwapChain::active->swapChainExtent.width, SwapChain::active->swapChainExtent.height);
	DIO.createImage(SwapChain::active->swapChainExtent.width, SwapChain::active->swapChainExtent.height);
}
