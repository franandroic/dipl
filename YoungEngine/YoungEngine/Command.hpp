#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "DeviceData.hpp"
#include "DeviceUtils.hpp"

#include "Device.hpp"
#include "SwapChain.hpp"
#include "RenderPass.hpp"
#include "Pipeline.hpp"
#include "Description.hpp"

class Command {

public:

	Command() = default;

	Command(Device *inDevice, SwapChain *inSwapChain, RenderPass *inRenderPass, Pipeline *inPipeline, Description *inDescription);

	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkCommandPool commandPool;

	std::vector<VkCommandBuffer> commandBuffers;

	void createFramebuffers(VkImageView colorImageView, VkImageView depthImageView);

	void createCommandPool(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

	void createCommandBuffers();

	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, uint32_t currentFrame, uint32_t indicesSize, VkBuffer vertexBuffer, VkBuffer indexBuffer);

private:

	Device *device;

	SwapChain *swapChain;

	RenderPass *renderPass;

	Pipeline *pipeline;

	Description *description;
};