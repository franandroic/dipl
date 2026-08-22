#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <array>

#include "DeviceData.hpp"

#include "Device.hpp"
#include "UniformBufferData.hpp"
#include "UniformBufferObject.hpp"

//Encapsulates the Vulkan objects related to descriptors
class Description {

public:

	//Constructor
	Description(Device &inDevice);

	//Copy and Move constructors
	Description(const Description &) = delete;
	Description &operator=(const Description &) = delete;
	Description(Description &&) = delete;
	Description &operator=(Description &&) = delete;

	//Member variables
	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
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