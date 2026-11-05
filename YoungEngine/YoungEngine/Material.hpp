#pragma once
#include "GLFWConfig.hpp"

#include <memory>
#include <vector>

#include "Device.hpp"
#include "DescriptorSet.hpp"
#include "DepthImageObject.hpp"
#include "PipelineDescription.hpp"
#include "Texture.hpp"
#include "MaterialPass.hpp"

class Material {

public:

	//Construction
	Material(std::vector<MaterialPass> inPasses);

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
	const MaterialPass &getPass(size_t pass) const { return passes.at(pass); }

	//The set of the textures a pass samples, VK_NULL_HANDLE if it samples none
	VkDescriptorSet getSet(size_t pass) const;

	//The bindings of the set of a pass, to describe a pipeline with
	std::vector<VkDescriptorSetLayoutBinding> bindings(size_t pass) const;

	//The attachments a pass renders into: the screen's (passed in) or those of its own texture
	PipelineDescription::RenderingFormats renderingFormats(size_t pass, const PipelineDescription::RenderingFormats &screenFormats) const;

	//The texture a pass renders into and its depth image, null if it renders into the screen (or has no depth)
	const Texture *getTarget(size_t pass) const { return targets.at(pass).get(); }
	const DepthImageObject *getTargetDepth(size_t pass) const { return targetDepths.at(pass).get(); }

	//(Re)creates the textures the passes render into if the extent of the window has changed since the last call
	//(or on the first call), and points the sets of the passes that sample them at the new ones.
	//Called every frame, before anything of the material is recorded.
	void prepareTargets(VkExtent2D extent);

private:

	static std::vector<MaterialPass> singlePass(const PipelineDescription::ShaderPair &shaders, const Texture *texture);
	static VkDescriptorSetLayoutBinding makeBinding(uint32_t binding);

	void validate() const;
	void writeTextures(size_t pass);
	void writePassInputs(size_t pass);

	std::vector<MaterialPass> passes;

	//One for each pass. Only a pass that samples something has a set. The textures don't change between
	//frames (the targets only when the window is resized), so a single set is shared by all the frames in flight.
	std::vector<std::unique_ptr<DescriptorSet>> sets;

	//One for each pass, null for a pass that renders into the screen
	std::vector<std::unique_ptr<Texture>> targets;
	std::vector<std::unique_ptr<DepthImageObject>> targetDepths;

	VkFormat depthFormat = VK_FORMAT_UNDEFINED;
	VkExtent2D targetExtent{ 0, 0 };
};