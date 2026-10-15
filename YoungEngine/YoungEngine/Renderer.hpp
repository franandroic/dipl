#pragma once
#include "GLFWConfig.hpp"

#include "Device.hpp"
#include "SwapChain.hpp"
#include "Synchronizer.hpp"

//Abstract class that represents responsibility to handle the render function
//Contains the render template method to be overriden by different types of renderers
class Renderer {

public:

	//Constructors
	Renderer(Device &inDevice, SwapChain &inSwapChain, size_t framesInFlight);
	virtual ~Renderer() = default;

	//Copy and Move constructors
	Renderer(const Renderer &) = delete;
	Renderer &operator=(const Renderer &) = delete;

	void render(bool externalResizeRequested);

protected:

	Device &device;
	SwapChain &swapChain;

	Synchronizer sync;
	size_t currentFrame = 0;

	virtual void updateFrame(size_t frame) = 0;
	virtual void recordFrame(uint32_t imageIndex, size_t frame) = 0;
	virtual void submitFrame(size_t frame, VkSemaphore imageAvailable, VkSemaphore renderFinished, VkFence inFlight) = 0;
	virtual void recreateSwapChain() = 0;
};