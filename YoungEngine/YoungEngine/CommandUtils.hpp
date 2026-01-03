#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class CommandUtils {

public:

	static VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool) {

		//A generic prologue to a single-use command recording.

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	static void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkDevice device, VkCommandPool commandPool, VkQueue queue) {

		//A generic epilogue to a single-use command recording.

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		//TODO: Look into creating a separate queue family specifically for transfer operations
		//by looking for the VK_QUEUE_TRANSFER_BIT
		vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
		//TODO: Swap the idle wait with fences, which would enable submitting multiple transfers
		//simultaneously and wait for all of them to finish
		vkQueueWaitIdle(queue);

		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	}
};