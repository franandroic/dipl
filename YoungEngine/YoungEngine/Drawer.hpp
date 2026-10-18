#pragma once

#include "GLFWConfig.hpp"

#include "Camera.hpp"

//Abstract class for something that draws one kind of object into the frame (meshes, lines...).
//The Renderer sets up the frame (command buffer, attachments, viewport) and calls each of its
//drawers, which only record their own pipeline binds and draw calls into that shared command buffer.
class Drawer {

public:

	virtual ~Drawer() = default;

	//Called once per frame, after the GPU has finished with the resources of this frame in flight.
	//Updates the data (uniform buffers, object state) the frame will use.
	virtual void update(size_t frame, const Camera &camera) = 0;

	//Records the draw calls into the command buffer, inside the active rendering scope.
	//Vulkan keeps state per command buffer, so a drawer binds everything it needs itself
	//(pipeline, descriptor sets, vertex/index buffers, push constants) and can't rely on
	//what a previous drawer left bound. Viewport and scissor are set by the Renderer.
	virtual void record(VkCommandBuffer commandBuffer, size_t frame) = 0;
};