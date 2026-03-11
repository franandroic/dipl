#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <array>

#include "DeviceData.hpp"

#include "Device.hpp"
#include "UniformBufferData.hpp"
#include "UniformBufferObject.hpp"

class Description {

public:

	Description() = default;

	Description(Device *inDevice);

	VkDescriptorSetLayout descriptorSetLayout;

	std::vector<VkDescriptorSet> descriptorSets;

	void createDescriptorSets(
		Device &device,
		VkDescriptorPool &descriptorPool,
		std::vector<UniformBufferObject> &UBOs,
		VkImageView textureImageView,
		VkSampler textureSampler
		);

private:

	void createDescriptorSetLayout(Device &device);
};