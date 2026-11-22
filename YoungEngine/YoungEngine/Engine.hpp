#pragma once
#include "GLMConfig.hpp"

#include "EngineUtility.hpp"

#include <cassert>
#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "Context.hpp"
#include "Window.hpp"
#include "InputCommand.hpp"
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
#include "LightSource.hpp"
#include "Scene.hpp"
#include "Prop.hpp"
#include "Axes.hpp"
#include "Renderer.hpp"
#include "ShaderCompiler.hpp"

#include "SimpleModelUD.hpp"
#include "SimpleLitUD.hpp"
#include "InPlaceYRotationUO.hpp"
#include "MovingLightUO.hpp"
#include "MouseDragUO.hpp"

/*
Represents the engine's global context.
Initializes and holds all the objects manipulated at runtime.
Created and called in main.
*/

class Engine {

public:

	//Construction
	Engine();
	~Engine();

	//Copy and Move constructors
	Engine(const Engine &) = delete;
	Engine &operator=(const Engine &) = delete;
	Engine(Engine &&) = delete;
	Engine &operator=(Engine &&) = delete;

	//Global access
	static inline Engine *active = nullptr;

	//Currently hard-coded values for initial window size and asset paths
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	const std::string MODEL_PATH = "models/suzanne.obj";
	const std::string FIRST_TEXTURE_PATH = "textures/Discr2DTex5.png";
	const std::string SECOND_TEXTURE_PATH = "textures/Discr2DTex3.png";

	const PipelineDescription::ShaderSource TOON_SHADERS{ "shaders/toon.vert", "shaders/toon.frag" };
	const PipelineDescription::ShaderSource NORMAL_SHADERS{ "shaders/normals.vert", "shaders/normals.frag" };
	const PipelineDescription::ShaderSource AXES_SHADERS{ "shaders/axes.vert", "shaders/axes.frag" };
	const PipelineDescription::ShaderSource HULL_SHADERS{ "shaders/hull.vert", "shaders/hull.frag" };
	const PipelineDescription::ShaderSource MASK_SHADERS{ "shaders/mask.vert", "shaders/mask.frag" };
	const PipelineDescription::ShaderSource ROBERTS_CROSS_SHADERS{ "shaders/fullscreen.vert", "shaders/robertscross.frag" };
	const PipelineDescription::ShaderSource PHONG_SHADERS{ "shaders/phong.vert", "shaders/phong.frag" };
	const PipelineDescription::ShaderSource SKETCH_SHADERS{ "shaders/fullscreen.vert", "shaders/sketch.frag" };

private:

	//Global value storage
	Context myContext;

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
	std::unique_ptr<ShaderCompiler> myShaderCompiler;

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
	Material &createMaterial(const PipelineDescription::ShaderSource &shaders, const Texture *texture = nullptr);
	Material &createMaterial(const std::vector<MaterialPass::Settings> &passes);
};