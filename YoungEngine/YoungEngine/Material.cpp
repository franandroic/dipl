#include "Material.hpp"

Material::Material(std::vector<MaterialPass> inPasses) : passes(std::move(inPasses)) {

	validate();

	sets.resize(passes.size());
	targets.resize(passes.size());
	targetDepths.resize(passes.size());

	for (size_t i = 0; i < passes.size(); i++) {

		if (passes[i].depth) {
			depthFormat = DeviceUtility::findDepthFormat(Device::active->physical);
		}

		//The loaded textures are written right away. The textures of earlier passes don't exist yet,
		//their bindings are written by prepareTargets, which runs before the set is first used.
		std::vector<VkDescriptorSetLayoutBinding> passBindings = bindings(i);
		if (passBindings.empty()) continue;

		sets[i] = std::make_unique<DescriptorSet>(passBindings);
		writeTextures(i);
	}
}

Material::Material(
	const PipelineDescription::ShaderPair &inShaders,
	const Texture *inTexture
) : Material(singlePass(inShaders, inTexture)) {
}

std::vector<MaterialPass> Material::singlePass(const PipelineDescription::ShaderPair &shaders, const Texture *texture) {

	MaterialPass pass;
	pass.shaders = shaders;
	if (texture) pass.textures.push_back(texture);

	return { pass };
}

void Material::validate() const {

	if (passes.empty()) {
		throw std::invalid_argument("A material needs at least one pass!");
	}

	for (size_t i = 0; i < passes.size(); i++) {

		for (const Texture *texture : passes[i].textures) {
			if (!texture) {
				throw std::invalid_argument("A material pass can't sample a null texture!");
			}
		}

		for (uint32_t input : passes[i].passInputs) {
			if (input >= i) {
				throw std::invalid_argument("A material pass can only sample the textures of earlier passes!");
			}
			if (!passes[input].rendersToTexture) {
				throw std::invalid_argument("A material pass samples a pass that doesn't render into a texture!");
			}
		}
	}
}

VkDescriptorSet Material::getSet(size_t pass) const {

	return sets.at(pass) ? sets[pass]->getHandle() : VK_NULL_HANDLE;
}

std::vector<VkDescriptorSetLayoutBinding> Material::bindings(size_t pass) const {

	//The loaded textures first, then the textures of earlier passes, all read by the fragment shader

	const MaterialPass &materialPass = passes.at(pass);
	uint32_t count = static_cast<uint32_t>(materialPass.textures.size() + materialPass.passInputs.size());

	std::vector<VkDescriptorSetLayoutBinding> result;
	result.reserve(count);

	for (uint32_t binding = 0; binding < count; binding++) {
		result.push_back(makeBinding(binding));
	}

	return result;
}

VkDescriptorSetLayoutBinding Material::makeBinding(uint32_t binding) {

	VkDescriptorSetLayoutBinding result{};
	result.binding = binding;
	result.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	result.descriptorCount = 1;
	result.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	result.pImmutableSamplers = nullptr;

	return result;
}

PipelineDescription::RenderingFormats Material::renderingFormats(size_t pass, const PipelineDescription::RenderingFormats &screenFormats) const {

	const MaterialPass &materialPass = passes.at(pass);
	if (!materialPass.rendersToTexture) return screenFormats;

	return {
		materialPass.format,
		materialPass.depth ? depthFormat : VK_FORMAT_UNDEFINED,
		VK_SAMPLE_COUNT_1_BIT
	};
}

void Material::prepareTargets(VkExtent2D extent) {

	//The extent only changes when the swap chain is recreated, which happens while the device is idle,
	//and this runs before anything is recorded in the frame after. So no frame in flight is using
	//the old textures or the sets that point at them when they're replaced here.

	if (extent.width == targetExtent.width && extent.height == targetExtent.height) return;
	targetExtent = extent;

	for (size_t i = 0; i < passes.size(); i++) {

		if (!passes[i].rendersToTexture) continue;

		targets[i] = std::make_unique<Texture>(extent.width, extent.height, passes[i].format);

		if (passes[i].depth) {
			targetDepths[i] = std::make_unique<DepthImageObject>(depthFormat);
			targetDepths[i]->createImage(extent.width, extent.height, VK_SAMPLE_COUNT_1_BIT);
		}
	}

	for (size_t i = 0; i < passes.size(); i++) {
		writePassInputs(i);
	}
}

void Material::writeTextures(size_t pass) {

	const std::vector<const Texture *> &textures = passes[pass].textures;

	for (uint32_t i = 0; i < static_cast<uint32_t>(textures.size()); i++) {
		sets[pass]->writeCombinedImageSampler(i, textures[i]->getImageView(), textures[i]->getSampler());
	}
}

void Material::writePassInputs(size_t pass) {

	const MaterialPass &materialPass = passes[pass];
	uint32_t firstBinding = static_cast<uint32_t>(materialPass.textures.size());

	for (uint32_t i = 0; i < static_cast<uint32_t>(materialPass.passInputs.size()); i++) {
		const Texture &target = *targets[materialPass.passInputs[i]];
		sets[pass]->writeCombinedImageSampler(firstBinding + i, target.getImageView(), target.getSampler());
	}
}
