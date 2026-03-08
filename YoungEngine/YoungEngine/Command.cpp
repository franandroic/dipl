#include "Command.hpp"

Command::Command(FrameBufferObject *inFBO, RenderPass *inRenderPass, Pipeline *inPipeline, Description *inDescription) {

	FBO = inFBO;
	renderPass = inRenderPass;
	pipeline = inPipeline;
	description = inDescription;
}

void Command::createCommandBuffers() {

	//Creating a command buffer to which we will record commands, before storing
	//them in a pool and submitting them to a queue to be executed.

	//One for each frame we want to be able to fill out before having to wait for the GPU.

	commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = FBO->swapChain->device->commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(FBO->swapChain->device->logical, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create command buffers!");
	}
}

void Command::recordCommandBuffer(
	VkCommandBuffer commandBuffer,
	uint32_t imageIndex,
	uint32_t currentFrame,
	uint32_t indicesSize,
	VkBuffer vertexBuffer,
	VkBuffer indexBuffer
	) {

	//This is where the recording of commands happens. Before recording we need to "begin"
	//the command buffer and the render pass.
	//During the recording we bind the vertex and index buffers that specify the data to be drawn,
	//as well as descriptor sets, which we use for things like uniform buffers and samplers.

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	//TODO: Look into different flags and how they're used
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass->renderPass;
	renderPassInfo.framebuffer = FBO->framebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = FBO->swapChain->swapChainExtent;

	//Ensure that the order of clear values is identical to the order of render pass attachments
	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
	clearValues[1].depthStencil = { 1.0f, 0 };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	//Thus begins the recording of commands to command buffers of the render pass
	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(FBO->swapChain->swapChainExtent.width);
	viewport.height = static_cast<float>(FBO->swapChain->swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = FBO->swapChain->swapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	VkBuffer vertexBuffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineLayout, 0, 1, &description->descriptorSets[currentFrame], 0, nullptr);

	//vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0); //For using just the vertex buffer
	vkCmdDrawIndexed(commandBuffer, indicesSize, 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to record command buffer!");
	}
}