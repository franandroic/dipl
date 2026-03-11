#include "Canvas.hpp"

Canvas::Canvas(SwapChain *inSwapChain, unsigned char *pixels, int texWidth, int texHeight, int texChannels) {


	renderPass = RenderPass(inSwapChain);

	description = Description(inSwapChain->device);
	createDescriptorPool(inSwapChain);

	FBO = FrameBufferObject(inSwapChain, &renderPass, pixels, texWidth, texHeight, texChannels);

	UBOs.resize(MAX_FRAMES_IN_FLIGHT);
	UBop = UniformBufferOperator(FBO.swapChain->swapChainExtent.width / (float)FBO.swapChain->swapChainExtent.height);
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		UBOs[i] = UniformBufferObject(
			FBO.swapChain->device,
			&UBdata,
			&UBop,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		UBOs[i].createBuffer();
	}

	description.createDescriptorSets(*FBO.swapChain->device, descriptorPool, UBOs, FBO.TIO.imageView, FBO.TIO.sampler);
}

void Canvas::createDescriptorPool(SwapChain *swapChain) {
	
	//Creating a descriptor pool, with a pool size for each descriptor set type and amount.

	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	poolInfo.flags = 0;

	if (vkCreateDescriptorPool(swapChain->device->logical, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor pool!");
	}
}