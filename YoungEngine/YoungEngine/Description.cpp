#include "Description.hpp"

Description::Description(Device &inDevice, const std::vector<VkDescriptorSetLayoutBinding> &bindings) : device(inDevice) {

	//Specifying what kind of descriptor sets we're going to bind to a pipeline, to which bindings
	//and to which shader stages. A list with no bindings makes a valid, empty layout.

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device.logical, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor set layout!");
	}
}

Description::~Description() {

	vkDestroyDescriptorSetLayout(device.logical, descriptorSetLayout, nullptr);
}

VkDescriptorSetLayoutBinding Description::makeBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stages) {

	VkDescriptorSetLayoutBinding layoutBinding{};
	layoutBinding.binding = binding;
	layoutBinding.descriptorType = type;
	layoutBinding.descriptorCount = 1;
	layoutBinding.stageFlags = stages;
	layoutBinding.pImmutableSamplers = nullptr;

	return layoutBinding;
}