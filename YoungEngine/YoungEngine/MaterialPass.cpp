#include "MaterialPass.hpp"

MaterialPass::MaterialPass(const Description &inDescription) : description(inDescription) {

	//The loaded textures are written right away. The textures of earlier passes don't exist yet,
	//their bindings are written through writeInput once they do, before the set is first used.

	std::vector<VkDescriptorSetLayoutBinding> passBindings = bindings();
	if (passBindings.empty()) return;

	set = std::make_unique<DescriptorSet>(passBindings);

	const std::vector<const Texture *> &textures = description.textures;
	for (uint32_t i = 0; i < static_cast<uint32_t>(textures.size()); i++) {
		set->writeCombinedImageSampler(i, textures[i]->getImageView(), textures[i]->getSampler());
	}
}

std::vector<VkDescriptorSetLayoutBinding> MaterialPass::bindings() const {

	//The loaded textures first, then the textures of earlier passes, all read by the fragment shader

	uint32_t count = static_cast<uint32_t>(description.textures.size() + description.passInputs.size());

	std::vector<VkDescriptorSetLayoutBinding> result;
	result.reserve(count);

	for (uint32_t binding = 0; binding < count; binding++) {
		result.push_back(makeBinding(binding));
	}

	return result;
}

VkDescriptorSetLayoutBinding MaterialPass::makeBinding(uint32_t binding) {

	VkDescriptorSetLayoutBinding result{};
	result.binding = binding;
	result.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	result.descriptorCount = 1;
	result.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	result.pImmutableSamplers = nullptr;

	return result;
}

PipelineDescription::RenderingFormats MaterialPass::renderingFormats(const PipelineDescription::RenderingFormats &screenFormats) const {

	//Known from the description alone, so it doesn't matter whether the target has been made yet

	if (!description.rendersToTexture) return screenFormats;
	return TextureRenderTarget::formatsFor(description.format, description.depth);
}

void MaterialPass::prepareTarget(VkExtent2D extent) {

	if (!description.rendersToTexture) return;
	target = std::make_unique<TextureRenderTarget>(extent, description.format, description.depth);
}

void MaterialPass::writeInput(uint32_t input, const Texture &texture) {

	uint32_t binding = static_cast<uint32_t>(description.textures.size()) + input;
	set->writeCombinedImageSampler(binding, texture.getImageView(), texture.getSampler());
}
