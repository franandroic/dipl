#pragma once
#include "GLFWConfig.hpp"

#include <cstdint>
#include <memory>
#include <vector>

#include "PipelineDescription.hpp"
#include "DescriptorSet.hpp"
#include "Texture.hpp"
#include "TextureRenderTarget.hpp"

//One pass of a material: one draw, with its own shaders, state and textures, either into the screen or into
//a render target of its own that later passes of the same material can sample.
//
//The textures a pass samples are bound to its fragment shader in this order:
//	first the loaded textures, layout(set = 1, binding = 0 ... textures.size() - 1)
//	then the textures of earlier passes, layout(set = 1, binding = textures.size() ...), in the order of passInputs
class MaterialPass {

public:

	struct Settings {

		PipelineDescription::ShaderPair shaders;
		bool hasFullscreenContext = false;

		//Loaded textures the pass samples
		std::vector<const Texture *> textures;

		//Indices of earlier passes of the same material, whose textures the pass samples.
		//Each of them has to render into a texture.
		std::vector<uint32_t> passInputs;

		//Whether the pass renders into a texture of its own instead of the screen.
		//The texture is the size of the window and is cleared and rendered anew every frame.
		bool rendersToTexture = false;
		VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
		bool depth = false;

		PipelineDescription::RasterState raster;
	};

	//Construction
	explicit MaterialPass(const Settings &inSettings);
	~MaterialPass() = default;

	//Copy and Move constructors
	MaterialPass(const MaterialPass &) = delete;
	MaterialPass &operator=(const MaterialPass &) = delete;
	MaterialPass(MaterialPass &&) = delete;
	MaterialPass &operator=(MaterialPass &&) = delete;

	const Settings &getSettings() const { return settings; }

	std::vector<VkDescriptorSetLayoutBinding> bindings() const;
	VkDescriptorSet getSet() const { return set ? set->getHandle() : VK_NULL_HANDLE; }
	
	const TextureRenderTarget *getTarget() const { return target.get(); }
	PipelineDescription::RenderingFormats renderingFormats(const PipelineDescription::RenderingFormats &screenFormats) const;

	void prepareTarget(VkExtent2D extent);
	void writeInput(uint32_t input, const Texture &texture);

private:

	static VkDescriptorSetLayoutBinding makeBinding(uint32_t binding);

	Settings settings;

	//Only a pass that samples something has a set. The textures don't change between frames (the targets of
	//earlier passes only when the window is resized), so a single set is shared by all the frames in flight.
	std::unique_ptr<DescriptorSet> set;

	std::unique_ptr<TextureRenderTarget> target;
};