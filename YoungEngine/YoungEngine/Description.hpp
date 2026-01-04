#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <array>

#include "DeviceData.hpp"

#include "Device.hpp"
#include "UniformBufferData.hpp"

class Description {

public:

	Description() = default;

	Description(Device *inDevice);

	VkDescriptorSetLayout descriptorSetLayout;

	VkDescriptorPool descriptorPool;

	std::vector<VkDescriptorSet> descriptorSets;

	void createDescriptorPool();

	void createDescriptorSets(std::vector<VkBuffer> uniformBuffers, VkImageView textureImageView, VkSampler textureSampler);

private:

	Device *device;

	void createDescriptorSetLayout();
};