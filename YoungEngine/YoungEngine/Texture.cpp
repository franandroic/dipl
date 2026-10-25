#include "Texture.hpp"

Texture::Texture(
	Device &inDevice,
	unsigned char *pixels,
	int inWidth, int inHeight, int inChannels
) : width(inWidth),
	height(inHeight),
	channels(inChannels),
	device(inDevice),
	TIO(inDevice)
{
	TIO.createImage(inDevice.commandPool, pixels, inWidth, inHeight, inChannels);
	TIO.createTextureSampler();
}

std::vector<VkDescriptorSetLayoutBinding> Texture::bindings() {

	return {
		Description::makeBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
	};
}

void Texture::createDescriptorSet(const Description &description) {

	descriptorSet = std::make_unique<DescriptorSet>(device, description);
	descriptorSet->writeCombinedImageSampler(0, TIO.imageView, TIO.sampler);
}

const DescriptorSet &Texture::getSet() const {

	if (!descriptorSet) {
		throw std::logic_error("The descriptor set of the texture hasn't been created!");
	}

	return *descriptorSet;
}