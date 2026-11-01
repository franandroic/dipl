#include "Command.hpp"

Command::Command(size_t count) {

	//Creating command buffers to which we will record commands, before submitting them to a queue
	//to be executed. One for each frame we want to be able to fill out before having to wait for the GPU.

	commandBuffers.resize(count);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = Device::active->commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if (vkAllocateCommandBuffers(Device::active->logical, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create command buffers!");
	}
}

VkCommandBuffer Command::begin(size_t frame) {

	//Resets the command buffer of the frame and begins recording into it

	VkCommandBuffer commandBuffer = commandBuffers[frame];

	vkResetCommandBuffer(commandBuffer, 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	//TODO: Look into different flags and how they're used
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recording command buffer!");
	}

	return commandBuffer;
}

void Command::end(size_t frame) {

	//Ends recording into the command buffer of the frame

	if (vkEndCommandBuffer(commandBuffers[frame]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to record command buffer!");
	}
}