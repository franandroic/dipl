#pragma once
#include "GLFWConfig.hpp"

#include <stdexcept>
#include <vector>

#include "Device.hpp"
#include "BufferObject.hpp"

//Wrapper for a single VkDescriptorSet.
class DescriptorSet {

public:

	//Construction
	DescriptorSet(Device &inDevice, const std::vector<VkDescriptorSetLayoutBinding> &bindings);
	~DescriptorSet();

	//Copy and move constructors
	DescriptorSet(const DescriptorSet &) = delete;
	DescriptorSet &operator=(const DescriptorSet &) = delete;
	DescriptorSet(DescriptorSet &&) = delete;
	DescriptorSet &operator=(DescriptorSet &&) = delete;

	VkDescriptorSet getHandle() const { return descriptorSet; }

	//Points a uniform buffer binding at the whole buffer. The buffer has to be created already.
	void writeUniformBuffer(uint32_t binding, const BufferObject &buffer);

	//Points a combined image sampler binding at an image view and a sampler.
	//The image is expected to be in the shader read only layout when it's sampled.
	void writeCombinedImageSampler(uint32_t binding, VkImageView imageView, VkSampler sampler);

private:

	Device &device;

	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
};