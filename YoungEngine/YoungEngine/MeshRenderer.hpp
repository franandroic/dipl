#pragma once

#include <stdexcept>

#include "Renderer.hpp"
#include "Window.hpp"
#include "Canvas.hpp"
#include "Pipeline.hpp"
#include "Mesh.hpp"
#include "Command.hpp"

class MeshRenderer : public Renderer {

public:

	MeshRenderer(
		Device &device,
		SwapChain &swapChain,
		Window &window,
		Canvas &canvas,
		Pipeline &pipeline,
		Mesh &mesh,
		Command &command,
		uint32_t indexCount,
		size_t framesInFlight
	);

protected:

	void updateFrame(size_t frame) override;
	void recordFrame(uint32_t imageIndex, size_t frame) override;
	void submitFrame(size_t frame, VkSemaphore imageAvailable, VkSemaphore renderFinished, VkFence inFlight) override;
	void recreateSwapChain() override;

private:

	Window &window;
	Canvas &canvas;
	Pipeline &pipeline;
	Mesh &mesh;
	Command &command;
	uint32_t indexCount;
};