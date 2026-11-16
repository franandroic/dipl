#include "Material.hpp"

Material::Material(const std::vector<MaterialPass::Settings> &settingsVector) {

	validate(settingsVector);

	passes.reserve(settingsVector.size());
	for (const MaterialPass::Settings &settings : settingsVector) {
		passes.push_back(std::make_unique<MaterialPass>(settings));
	}
}

Material::Material(
	const PipelineDescription::ShaderPair &inShaders,
	const Texture *inTexture
) : Material(singlePass(inShaders, inTexture)) {
}

std::vector<MaterialPass::Settings> Material::singlePass(const PipelineDescription::ShaderPair &shaders, const Texture *texture) {

	MaterialPass::Settings pass;
	pass.shaders = shaders;
	if (texture) pass.textures.push_back(texture);

	return { pass };
}

void Material::validate(const std::vector<MaterialPass::Settings> &settingsVector) {

	if (settingsVector.empty()) {
		throw std::invalid_argument("A material needs at least one pass!");
	}

	for (size_t i = 0; i < settingsVector.size(); i++) {

		for (const Texture *texture : settingsVector[i].textures) {
			if (!texture) {
				throw std::invalid_argument("A material pass can't sample a null texture!");
			}
		}

		for (uint32_t input : settingsVector[i].passInputs) {
			if (input >= i) {
				throw std::invalid_argument("A material pass can only sample the textures of earlier passes!");
			}
			if (!settingsVector[input].rendersOffscreen) {
				throw std::invalid_argument("A material pass samples a pass that doesn't render into a texture!");
			}
		}
	}
}

void Material::prepareTargets(VkExtent2D extent) {

	//The extent only changes when the swap chain is recreated, which happens while the device is idle,
	//and this runs before anything is recorded in the frame after. So no frame in flight is using
	//the old targets or the sets that point at them when they're replaced here.

	if (extent.width == targetExtent.width && extent.height == targetExtent.height) return;
	targetExtent = extent;

	for (const auto &pass : passes) {
		pass->prepareTarget(extent);
	}

	//Every pass that samples earlier passes is pointed at the textures of their new targets
	for (const auto &pass : passes) {

		const std::vector<uint32_t> &inputs = pass->getSettings().passInputs;

		for (uint32_t i = 0; i < static_cast<uint32_t>(inputs.size()); i++) {
			pass->writeInput(i, passes[inputs[i]]->getTarget()->getTexture());
		}
	}
}
