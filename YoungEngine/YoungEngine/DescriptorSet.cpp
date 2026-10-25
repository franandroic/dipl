#include "DescriptorSet.hpp"

DescriptorSet::DescriptorSet(Device &inDevice, const Description &description) : device(inDevice) {

	//Allocating a single set with the layout of the description from the pool of the device.

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = device.descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &description.descriptorSetLayout;

	if (vkAllocateDescriptorSets(device.logical, &allocInfo, &descriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate descriptor set! (Is the descriptor pool exhausted? See DeviceData.hpp)");
	}
}

DescriptorSet::~DescriptorSet() {

	//Returns the set to the pool (the pool is created with the free flag to allow this).

	vkFreeDescriptorSets(device.logical, device.descriptorPool, 1, &descriptorSet);
}

void DescriptorSet::writeUniformBuffer(uint32_t binding, const BufferObject &buffer) {

	if (buffer.buffer == VK_NULL_HANDLE) {
		throw std::invalid_argument("Can't write a buffer that hasn't been created to a descriptor set!");
	}

	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = buffer.buffer;
	bufferInfo.offset = 0;
	bufferInfo.range = buffer.size;

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = descriptorSet;
	write.dstBinding = binding;
	write.dstArrayElement = 0;
	write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	write.descriptorCount = 1;
	write.pBufferInfo = &bufferInfo;
	write.pImageInfo = nullptr;
	write.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(device.logical, 1, &write, 0, nullptr);
}

void DescriptorSet::writeCombinedImageSampler(uint32_t binding, VkImageView imageView, VkSampler sampler) {

	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = imageView;
	imageInfo.sampler = sampler;

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.dstSet = descriptorSet;
	write.dstBinding = binding;
	write.dstArrayElement = 0;
	write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	write.descriptorCount = 1;
	write.pBufferInfo = nullptr;
	write.pImageInfo = &imageInfo;
	write.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(device.logical, 1, &write, 0, nullptr);
}