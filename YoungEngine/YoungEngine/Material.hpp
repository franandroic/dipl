#pragma once
#include "GLFWConfig.hpp"

#include <memory>
#include <stdexcept>
#include <vector>

#include "PipelineDescription.hpp"
#include "Texture.hpp"
#include "MaterialPass.hpp"

//What an object looks like: a chain of one or more passes (see MaterialPass), drawn in order.
//Each pass owns its own resources; the material does what spans passes: it checks that the passes refer to
//each other correctly, and it connects the passes that sample earlier passes to the targets of those passes.
class Material {

public:

	//Construction
	explicit Material(const std::vector<MaterialPass::Description> &descriptions);

	//A single pass into the screen, sampling the texture if there is one
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

	size_t passCount() const { return passes.size(); }
	const MaterialPass &getPass(size_t pass) const { return *passes.at(pass); }

	void prepareTargets(VkExtent2D extent);

private:

	static std::vector<MaterialPass::Description> singlePass(const PipelineDescription::ShaderPair &shaders, const Texture *texture);
	static void validate(const std::vector<MaterialPass::Description> &descriptions);

	std::vector<std::unique_ptr<MaterialPass>> passes;

	VkExtent2D targetExtent{ 0, 0 };
};
