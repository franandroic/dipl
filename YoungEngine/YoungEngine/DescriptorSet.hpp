#pragma once
#include "GLFWConfig.hpp"

#include <stdexcept>

#include "Device.hpp"
#include "Description.hpp"
#include "BufferObject.hpp"

//Wrapper for a single VkDescriptorSet.
//It's allocated from the descriptor pool of the Device and returned to it on destruction,
//so it has to be destroyed before the Device is. The set is allocated with the layout of a Description,
//and the descriptors in it are filled out with the write functions. The Description is only needed
//during construction: the set stays valid after it's destroyed.
//
//A DescriptorSet belongs to the object whose data it points at (the camera, a texture...), not to the pipeline
//it's bound with.
//
//Updating a set that the GPU is still using is not allowed, so sets that change
//are kept one per frame in flight and written once, when they're set up.
class DescriptorSet {

public:

	//Construction
	DescriptorSet(Device &inDevice, const Description &description);
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

	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
};