#pragma once
#include "GLMConfig.hpp"

#include "EngineUtility.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "Window.hpp"
#include "Vertex.hpp"
#include "Device.hpp"
#include "SwapChain.hpp"
#include "PipelineCache.hpp"
#include "PipelineDescription.hpp"
#include "ModelLoader.hpp"
#include "ImageLoader.hpp"
#include "Loader.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "Camera.hpp"
#include "Scene.hpp"
#include "Prop.hpp"
#include "Axes.hpp"
#include "Renderer.hpp"

#include "SimpleModelUD.hpp"
#include "InPlaceYRotationUO.hpp"


/*
Represents the engine's global context.
Initializes and holds all the objects manipulated at runtime.
Created and called in main.
*/

class Engine {

public:

	//Construction
	Engine();
	~Engine() = default;

	//Copy and Move constructors
	Engine(const Engine &) = delete;
	Engine &operator=(const Engine &) = delete;
	Engine(Engine &&) = delete;
	Engine &operator=(Engine &&) = delete;

	//Currently hard-coded values for initial window size and asset paths
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	const std::string MODEL_PATH = "models/suzanne.obj";
	const std::string FIRST_TEXTURE_PATH = "textures/Discr2DTex5.png";
	const std::string SECOND_TEXTURE_PATH = "textures/Discr2DTex3.png";

	const PipelineDescription::ShaderPair TEXTURED_SHADERS{ "shaders/toon.vert.spv", "shaders/toon.frag.spv" };
	const PipelineDescription::ShaderPair UNTEXTURED_SHADERS{ "shaders/normals.vert.spv", "shaders/normals.frag.spv" };
	const PipelineDescription::ShaderPair AXES_SHADERS{ "shaders/axes.vert.spv", "shaders/axes.frag.spv" };

private:

	//Specific loaders and a generic loader
	ModelLoader myModelLoader;
	ImageLoader myImageLoader;
	Loader myLoader;

	//Essential objects that are part of the context
	Window myWindow;
	EngineUtility myEngineUtility;

	//Globally accessible singletons
	std::unique_ptr<Device> myDevice;
	std::unique_ptr<SwapChain> mySwapChain;
	std::unique_ptr<PipelineCache> myPipelineCache;

	//Vectors of things that props can be constructed with
	std::vector<std::unique_ptr<Mesh>> myMeshes;
	std::vector<std::unique_ptr<Texture>> myTextures;
	std::vector<std::unique_ptr<Material>> myMaterials;

	//Where things happen and what makes them happen
	std::unique_ptr<Scene> myScene;
	std::unique_ptr<Renderer> myRenderer;

public:

	void run();

private:
	
	void initialize();
	void renderLoop();

	Mesh &loadMesh(const std::string &path);
	Texture &loadTexture(const std::string &path);
	Material &createMaterial(const PipelineDescription::ShaderPair &shaders, const Texture *texture = nullptr);
};