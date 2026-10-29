#pragma once
#include "GLFWConfig.hpp"

#include <memory>
#include <vector>

#include "Device.hpp"
#include "DescriptorSet.hpp"
#include "PipelineDescription.hpp"
#include "Texture.hpp"
#include "UniformData.hpp"
#include "UniformOperator.hpp"
#include "Uniformity.hpp"

class Material {

public:

	//Construction
	Material(
		Device &inDevice,
		const PipelineDescription::ShaderPair &inShaders,
		std::unique_ptr<UniformData> inUniformData,
		std::unique_ptr<UniformOperator> inUniformOperator,
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
	const Uniformity &getUniformity() const { return uniformity; }

	//The bindings of the descriptor set of the material, to describe a pipeline with
	std::vector<VkDescriptorSetLayoutBinding> bindings() const;

	//Runs the operator on the uniform data, once per frame
	void update(size_t frame);

	//Binds the sets of the material and of its uniform variables and records its push constants into a command buffer
	//that's being recorded. The pipeline has to be bound, and it has to be one made from a description
	//that the material gave (the layout of the sets has to be the same).
	void apply(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t frame) const;

private:

	//Binding 0 is the texture, read in the fragment shader
	static constexpr uint32_t TEXTURE_BINDING = 0;
	static VkDescriptorSetLayoutBinding makeBinding();

	Device &device;

	PipelineDescription::ShaderPair shaders;
	const Texture *texture;
	Uniformity uniformity;

	//Only a material with a texture has a set. The texture doesn't change between frames,
	//so a single set is shared by all the frames in flight.
	std::unique_ptr<DescriptorSet> textureSet;
};