#pragma once
#include "GLFWConfig.hpp"

#include <cstdint>
#include <vector>

#include "PipelineDescription.hpp"
#include "Texture.hpp"

//One pass of a material: one draw, with its own shaders, state and textures, either into the screen or into
//a texture of its own (owned by the material) that later passes of the same material can sample.
//
//The textures a pass samples are bound to its fragment shader in this order:
//	first the loaded textures, layout(set = 1, binding = 0 ... textures.size() - 1)
//	then the textures of earlier passes, layout(set = 1, binding = textures.size() ...), in the order of passInputs
struct MaterialPass {

	//What the pass draws
	enum class Geometry {
		Mesh,		//the mesh of every prop that uses the material
		Fullscreen	//one triangle covering the whole target, once per material
	};

	PipelineDescription::ShaderPair shaders;
	Geometry geometry = Geometry::Mesh;

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