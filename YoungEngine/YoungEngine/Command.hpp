#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "DeviceData.hpp"
#include "DeviceUtils.hpp"

#include "RenderPass.hpp"
#include "Pipeline.hpp"
#include "Description.hpp"
#include "FrameBufferObject.hpp"

//Wrapper for Vulkan Command Buffers
class Command {

public:

	//Construction
	Command() = default;

	//Member variables
	std::vector<VkCommandBuffer> commandBuffers;

	void createCommandBuffers(Device &device);

	void recordCommandBuffer(
		VkCommandBuffer commandBuffer,
		uint32_t imageIndex,
		uint32_t currentFrame,
		uint32_t indicesSize,
		VkBuffer vertexBuffer,
		VkBuffer indexBuffer,
		FrameBufferObject &FBO,
		RenderPass &renderPass,
		Pipeline &pipeline,
		Description &description
		);
};