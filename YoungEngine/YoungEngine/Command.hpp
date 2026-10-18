#pragma once
#include "GLFWConfig.hpp"

#include <vector>
#include <stdexcept>

#include "Device.hpp"

//Wrapper for Vulkan command buffers: one for each frame in flight, so that a frame can be
//recorded while the GPU is still executing the previous ones.
//It only handles the lifecycle of the buffers. What gets recorded into them is up to the renderer.
class Command {

public:

	//Construction
	Command(Device &device, size_t count);
	~Command() = default;

	//Copy and Move constructors
	Command(const Command &) = delete;
	Command &operator=(const Command &) = delete;
	Command(Command &&) = delete;
	Command &operator=(Command &&) = delete;

	VkCommandBuffer getBuffer(size_t frame) const { return commandBuffers[frame]; }
	size_t count() const { return commandBuffers.size(); }

	//Resets the command buffer of the frame and begins recording into it
	VkCommandBuffer begin(size_t frame);

	//Ends recording into the command buffer of the frame
	void end(size_t frame);

private:

	std::vector<VkCommandBuffer> commandBuffers;
};