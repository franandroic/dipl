#pragma once
#include "GLFWConfig.hpp"

#include <memory>
#include <vector>
#include <stdexcept>

#include "ImageUtils.hpp"

#include "Device.hpp"
#include "SwapChain.hpp"
#include "Window.hpp"
#include "Synchronizer.hpp"
#include "Command.hpp"
#include "RenderTarget.hpp"
#include "Camera.hpp"
#include "Drawer.hpp"

//Orchestrates a frame: synchronization, acquiring and presenting swap chain images, the command buffer,
//the attachments and the rendering scope. What actually gets drawn is delegated to Drawers,
//so objects of different kinds can be rendered into the same frame.
//
//It owns the RenderTarget (the attachments all of the pipelines of the frame render to),
//which is why pipelines are created from it.
class Renderer {

public:

	//Constructors
	Renderer(
		Device &inDevice,
		SwapChain &inSwapChain,
		Window &inWindow,
		Camera &inCamera,
		size_t framesInFlight
	);
	~Renderer() = default;

	//Copy and Move constructors
	Renderer(const Renderer &) = delete;
	Renderer &operator=(const Renderer &) = delete;
	Renderer(Renderer &&) = delete;
	Renderer &operator=(Renderer &&) = delete;

	//Drawers are called in the order they were added. The renderer takes ownership.
	void addDrawer(std::unique_ptr<Drawer> drawer);

	const RenderTarget &getRenderTarget() const { return renderTarget; }

	void render(bool externalResizeRequested);

protected:

	Device &device;
	SwapChain &swapChain;
	Window &window;
	Camera &camera;

	RenderTarget renderTarget;
	Synchronizer sync;
	Command command;

	std::vector<std::unique_ptr<Drawer>> drawers;
	size_t currentFrame = 0;

	void updateFrame(size_t frame);
	void recordFrame(VkCommandBuffer commandBuffer, uint32_t imageIndex, size_t frame);
	void submitFrame(size_t frame, VkSemaphore imageAvailable, VkSemaphore renderFinished, VkFence inFlight);
	void recreateSwapChain();
};