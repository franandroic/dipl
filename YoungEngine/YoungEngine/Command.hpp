#pragma once
#include "GLFWConfig.hpp"

#include "DeviceData.hpp"
#include "DeviceUtils.hpp"

#include "Pipeline.hpp"
#include "Description.hpp"
#include "FrameBufferObject.hpp"
#include "Uniformity.hpp"

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
		Pipeline &pipeline,
		Description &description,
		const Uniformity &uniformity
		);
};