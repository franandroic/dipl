#pragma once
#include "GLMConfig.hpp"

#include "EngineUtility.hpp"

#include "Window.hpp"
#include "Vertex.hpp"
#include "Device.hpp"
#include "SwapChain.hpp"
#include "PipelineCache.hpp"
#include "ModelLoader.hpp"
#include "ImageLoader.hpp"
#include "Loader.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "Camera.hpp"
#include "GameObject.hpp"
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
	const std::string TEXTURE_PATH = "textures/Discr2DTex5.png";

	const std::string VERT_SHADER_PATH = "shaders/vert.spv";
	const std::string FRAG_SHADER_PATH = "shaders/frag.spv";

	//VERTICES
	std::vector<Vertex> vertices;

	//INDICES
	std::vector<uint32_t> indices;

private:

	ModelLoader myModelLoader;
	ImageLoader myImageLoader;
	Loader myLoader;

	Window myWindow;
	EngineUtility myEngineUtility;

	std::unique_ptr<Device> myDevice;
	std::unique_ptr<SwapChain> mySwapChain;
	std::unique_ptr<PipelineCache> myPipelineCache;
	std::unique_ptr<Texture> myTexture;
	std::unique_ptr<Camera> myCamera;
	std::unique_ptr<Renderer> myRenderer;

public:

	void run();

private:
	
	void initVulkan();
	void mainLoop();
};