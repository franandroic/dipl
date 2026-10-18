#include "Renderer.hpp"

Renderer::Renderer(Device &inDevice, SwapChain &inSwapChain, size_t framesInFlight)
	: device(inDevice), swapChain(inSwapChain), sync(inDevice, framesInFlight) {
}

void Renderer::render(bool externalResizeRequested) {

	sync.stop(currentFrame);

	updateFrame(currentFrame);

	uint32_t imageIndex;
	SwapChain::AcquireResult acquireResult = swapChain.acquireNextImage(
		sync.imageAvailable(currentFrame),
		imageIndex
	);
	if (acquireResult == SwapChain::AcquireResult::OutOfDate) {
		recreateSwapChain();
		return;
	}

	sync.go(currentFrame);

	recordFrame(imageIndex, currentFrame);
	submitFrame(
		currentFrame,
		sync.imageAvailable(currentFrame),
		sync.renderFinished(currentFrame),
		sync.inFlightFence(currentFrame)
		);

	bool needsRecreate = swapChain.present(sync.renderFinished(currentFrame), imageIndex);
	if (needsRecreate || externalResizeRequested) {
		recreateSwapChain();
	}

	currentFrame = (currentFrame + 1) % sync.count();
}