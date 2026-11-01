#pragma once
#include "GLFWConfig.hpp"

#include <stdexcept>
#include <vector>

#include "Device.hpp"
#include "BufferObject.hpp"

//Wrapper for a single VkDescriptorSet. Stores a set and its layout.
class DescriptorSet {

public:

	//Construction
	explicit DescriptorSet(const std::vector<VkDescriptorSetLayoutBinding> &bindings);
	~DescriptorSet();

	//Copy and move constructors
	explicit DescriptorSet(const DescriptorSet &) = delete;
	DescriptorSet &operator=(const DescriptorSet &) = delete;
	DescriptorSet(DescriptorSet &&) = delete;
	DescriptorSet &operator=(DescriptorSet &&) = delete;

	VkDescriptorSet getHandle() const { return descriptorSet; }

	void writeUniformBuffer(uint32_t binding, const BufferObject &buffer) const;
	void writeCombinedImageSampler(uint32_t binding, VkImageView imageView, VkSampler sampler) const;

private:

	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
};