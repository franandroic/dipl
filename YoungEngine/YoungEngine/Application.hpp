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
#include "GameObject.hpp"
#include "Axes.hpp"
#include "Renderer.hpp"

#include "SimpleModelUD.hpp"
#include "InPlaceYRotationUO.hpp"

//THE MAIN APPLICATION CLASS
class Application {

public:

	//Construction
	Application();
	~Application() = default;

	//Copy and Move constructors
	Application(const Application &) = delete;
	Application &operator=(const Application &) = delete;
	Application(Application &&) = delete;
	Application &operator=(Application &&) = delete;

	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	const std::string MODEL_PATH = "models/suzanne.obj";
	const std::string FIRST_TEXTURE_PATH = "textures/Discr2DTex5.png";
	const std::string SECOND_TEXTURE_PATH = "textures/Discr2DTex3.png";

	const PipelineDescription::ShaderPair TEXTURED_SHADERS{ "shaders/toon.vert.spv", "shaders/toon.frag.spv" };
	const PipelineDescription::ShaderPair UNTEXTURED_SHADERS{ "shaders/normals.vert.spv", "shaders/normals.frag.spv" };
	const PipelineDescription::ShaderPair AXES_SHADERS{ "shaders/axes.vert.spv", "shaders/axes.frag.spv" };

private:

	ModelLoader myModelLoader;
	ImageLoader myImageLoader;
	Loader myLoader;

	Window myWindow;
	EngineUtility myEngineUtility;

	std::unique_ptr<Device> myDevice;
	std::unique_ptr<SwapChain> mySwapChain;
	std::unique_ptr<PipelineCache> myPipelineCache;

	std::vector<std::unique_ptr<Mesh>> myMeshes;
	std::vector<std::unique_ptr<Texture>> myTextures;
	std::vector<std::unique_ptr<Material>> myMaterials;

	std::unique_ptr<Scene> myScene;
	std::unique_ptr<Renderer> myRenderer;

public:

	void run();

private:
	
	void initVulkan();
	void mainLoop();

	Mesh &loadMesh(const std::string &path);
	Texture &loadTexture(const std::string &path);
	Material &createMaterial(const PipelineDescription::ShaderPair &shaders, const Texture *texture = nullptr);
};