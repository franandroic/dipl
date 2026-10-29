#include "Material.hpp"

Material::Material(
	Device &inDevice,
	const PipelineDescription::ShaderPair &inShaders,
	const Texture *inTexture
) : device(inDevice),
	shaders(inShaders),
	texture(inTexture) {

	if (texture) {
		textureSet = std::make_unique<DescriptorSet>(device, bindings());
		textureSet->writeCombinedImageSampler(TEXTURE_BINDING, texture->getImageView(), texture->getSampler());
	}
}

std::vector<VkDescriptorSetLayoutBinding> Material::bindings() const {

	if (!texture) return {};

	return { makeBinding() };
}

VkDescriptorSetLayoutBinding Material::makeBinding() {

	VkDescriptorSetLayoutBinding binding{};
	binding.binding = TEXTURE_BINDING;
	binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	binding.descriptorCount = 1;
	binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	binding.pImmutableSamplers = nullptr;

	return binding;
}
