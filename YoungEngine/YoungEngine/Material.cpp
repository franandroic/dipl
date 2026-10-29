#include "Material.hpp"

Material::Material(
	Device &inDevice,
	const PipelineDescription::ShaderPair &inShaders,
	std::unique_ptr<UniformData> inUniformData,
	std::unique_ptr<UniformOperator> inUniformOperator,
	const Texture *inTexture
) : device(inDevice),
	shaders(inShaders),
	texture(inTexture),
	uniformity(inDevice, std::move(inUniformData), std::move(inUniformOperator), MAX_FRAMES_IN_FLIGHT) {

	if (texture) {
		textureSet = std::make_unique<DescriptorSet>(device, bindings());
		textureSet->writeCombinedImageSampler(TEXTURE_BINDING, texture->getImageView(), texture->getSampler());
	}
}

std::vector<VkDescriptorSetLayoutBinding> Material::bindings() const {

	if (!texture) return {};

	return { makeBinding() };
}

void Material::update(size_t frame) {

	uniformity.update(frame);
}

void Material::apply(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t frame) const {

	//Each tier of descriptors is bound at its own set number. The texture's set is the same in every frame,
	//while the uniform buffers of the object are one per frame in flight.

	if (textureSet) {
		VkDescriptorSet set = textureSet->getHandle();
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, PipelineDescription::DescriptorTier::Material, 1, &set, 0, nullptr);
	}

	if (uniformity.hasDescriptorSets()) {
		VkDescriptorSet set = uniformity.getSet(frame).getHandle();
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, PipelineDescription::DescriptorTier::Object, 1, &set, 0, nullptr);
	}

	uniformity.recordPushConstants(commandBuffer, pipelineLayout);
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
