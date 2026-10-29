#include "DescriptorSet.hpp"

DescriptorSet::DescriptorSet(Device &inDevice, const std::vector<VkDescriptorSetLayoutBinding> &bindings) : device(inDevice) {

	//Specifying what kind of set we're going to create, which bindings it has, of what type and for which
	//shader stages (the descriptor set layout), and allocating a single set of it from the pool of the device.

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device.logical, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor set layout!");
	}

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = device.descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &descriptorSetLayout;

	if (vkAllocateDescriptorSets(device.logical, &allocInfo, &descriptorSet) != VK_SUCCESS) {
		vkDestroyDescriptorSetLayout(device.logical, descriptorSetLayout, nullptr);
		throw std::runtime_error("Failed to allocate descriptor set! (Is the descriptor pool exhausted? See DeviceData.hpp)");
	}
}

DescriptorSet::~DescriptorSet() {

	//Returns the set to the pool (the pool is created with the free flag to allow this).

	vkFreeDescriptorSets(device.logical, device.descriptorPool, 1, &descriptorSet);
	vkDestroyDescriptorSetLayout(device.logical, descriptorSetLayout, nullptr);
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