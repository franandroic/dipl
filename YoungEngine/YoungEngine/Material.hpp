#pragma once
#include "GLFWConfig.hpp"

#include <memory>
#include <vector>

#include "Device.hpp"
#include "DescriptorSet.hpp"
#include "PipelineDescription.hpp"
#include "Texture.hpp"

class Material {

public:

	//Construction
	Material(
		const PipelineDescription::ShaderPair &inShaders,
		const Texture *inTexture = nullptr
	);
	~Material() = default;

	//Copy and Move constructors
	Material(const Material &) = delete;
	Material &operator=(const Material &) = delete;
	Material(Material &&) = delete;
	Material &operator=(Material &&) = delete;

	const PipelineDescription::ShaderPair &getShaders() const { return shaders; }
	const Texture *getTexture() const { return texture; }
	VkDescriptorSet getSet() const { return textureSet ? textureSet->getHandle() : VK_NULL_HANDLE; }

	std::vector<VkDescriptorSetLayoutBinding> bindings() const;

private:

	static constexpr uint32_t TEXTURE_BINDING = 0;
	static VkDescriptorSetLayoutBinding makeBinding();

	PipelineDescription::ShaderPair shaders;

	const Texture *texture;
	std::unique_ptr<DescriptorSet> textureSet;
};