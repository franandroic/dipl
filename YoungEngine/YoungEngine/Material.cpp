#include "Material.hpp"

Material::Material(
	const PipelineDescription::ShaderPair &inShaders,
	const Texture *inTexture
) : shaders(inShaders),
	texture(inTexture) {

	//Only a material with a texture has a set. The texture doesn't change between frames,
	//so a single set is shared by all the frames in flight.

	if (texture) {
		textureSet = std::make_unique<DescriptorSet>(bindings());
		textureSet->writeCombinedImageSampler(TEXTURE_BINDING, texture->getImageView(), texture->getSampler());
	}
}

std::vector<VkDescriptorSetLayoutBinding> Material::bindings() const {

	//The bindings of the descriptor set of the material, to describe a pipeline with

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
