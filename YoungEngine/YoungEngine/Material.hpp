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

	//The bindings of the descriptor set of the material, to describe a pipeline with
	std::vector<VkDescriptorSetLayoutBinding> bindings() const;

	//The set to bind at DescriptorTier::Material, or VK_NULL_HANDLE if the material has no texture
	VkDescriptorSet getSet() const { return textureSet ? textureSet->getHandle() : VK_NULL_HANDLE; }

private:

	//Binding 0 is the texture, read in the fragment shader
	static constexpr uint32_t TEXTURE_BINDING = 0;
	static VkDescriptorSetLayoutBinding makeBinding();

	PipelineDescription::ShaderPair shaders;
	const Texture *texture;

	//Only a material with a texture has a set. The texture doesn't change between frames,
	//so a single set is shared by all the frames in flight.
	std::unique_ptr<DescriptorSet> textureSet;
};