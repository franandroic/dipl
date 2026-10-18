#include "Canvas.hpp"

Canvas::Canvas(
	Device &inDevice,
	unsigned char *pixels,
	int texWidth, int texHeight, int texChannels,
	std::unique_ptr<UniformData> inUniformData,
	std::unique_ptr<UniformOperator> inUniformOperator
) : description(inDevice),
	texture(inDevice, pixels, texWidth, texHeight, texChannels),
	uniformity(inDevice, std::move(inUniformData), std::move(inUniformOperator), MAX_FRAMES_IN_FLIGHT),
	device(inDevice)
{
	createDescriptorPool();

	UBOs.reserve(MAX_FRAMES_IN_FLIGHT);
	
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		UBOs.push_back(std::make_unique<UniformBufferObject>(
			device,
			sizeof(Camera::UniformBlock),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		));
		UBOs[i]->createBuffer();
	}

	description.createDescriptorSets(descriptorPool, UBOs, texture.getImageView(), texture.getSampler());
}

Canvas::~Canvas() {
	
	vkDestroyDescriptorPool(device.logical, descriptorPool, nullptr);
}

void Canvas::createDescriptorPool() {
	
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

	if (vkCreateDescriptorPool(device.logical, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor pool!");
	}
}