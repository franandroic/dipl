#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "DeviceData.hpp"
#include "DeviceUtils.hpp"

#include "RenderPass.hpp"
#include "Pipeline.hpp"
#include "Description.hpp"
#include "FrameBufferObject.hpp"

class Command {

public:

	Command() = default;

	Command(FrameBufferObject *inFBO, RenderPass *inRenderPass, Pipeline *inPipeline, Description *inDescription);

	std::vector<VkCommandBuffer> commandBuffers;

	void createCommandBuffers();

	void recordCommandBuffer(
		VkCommandBuffer commandBuffer,
		uint32_t imageIndex,
		uint32_t currentFrame,
		uint32_t indicesSize,
		VkBuffer vertexBuffer,
		VkBuffer indexBuffer
		);

private:

	FrameBufferObject *FBO;

	RenderPass *renderPass;

	Pipeline *pipeline;

	Description *description;
};