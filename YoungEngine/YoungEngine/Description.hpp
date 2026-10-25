#pragma once
#include "GLFWConfig.hpp"

#include <cstdint>
#include <vector>

#include "Device.hpp"

//The tiers the descriptors are split into, from the least to the most frequently changing.
//The value is the set number: both the N in "set = N" in the shaders and the position of the
//layout in the pipeline layout.
namespace DescriptorTier {
	constexpr uint32_t Frame = 0;		//shared by everything drawn in a frame (the camera)
	constexpr uint32_t Material = 1;	//what an object looks like (the texture)
	constexpr uint32_t Object = 2;		//data of a single object that goes through a uniform buffer
	constexpr uint32_t Count = 3;
}

//Describes the shape of one kind of descriptor set: which bindings it has, of what type and for
//which shader stages (the descriptor set layout). It doesn't hold any descriptor sets: those belong
//to the objects whose data they point at, and are allocated from a Description (see DescriptorSet).
//
//Pipelines create their Descriptions from binding lists given to them from the outside. A set can be bound
//with any pipeline whose layout for that set number was created from an identical binding list.
class Description {

public:

	//Construction
	Description(Device &inDevice, const std::vector<VkDescriptorSetLayoutBinding> &bindings);
	~Description();

	//Copy and Move constructors
	Description(const Description &) = delete;
	Description &operator=(const Description &) = delete;
	Description(Description &&) = delete;
	Description &operator=(Description &&) = delete;

	//Shorthand for filling out a layout binding that holds a single descriptor
	static VkDescriptorSetLayoutBinding makeBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stages);

	//Member variables
	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

private:

	Device &device;
};