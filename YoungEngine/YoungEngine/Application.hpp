#pragma once
#include "GLMConfig.hpp"

#include "EngineUtility.hpp"

#include "Window.hpp"
#include "Vertex.hpp"
#include "Device.hpp"
#include "SwapChain.hpp"
#include "Pipeline.hpp"
#include "ModelLoader.hpp"
#include "ImageLoader.hpp"
#include "Loader.hpp"
#include "Mesh.hpp"
#include "Canvas.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"

#include "SimpleModelUD.hpp"
#include "InPlaceYRotationUO.hpp"
#include "MeshDrawer.hpp"

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
	std::unique_ptr<Pipeline> myPipeline;
	std::unique_ptr<Canvas> myCanvas;
	Camera myCamera;
	std::unique_ptr<Mesh> myMesh;
	std::unique_ptr<Renderer> myRenderer;

public:

	void run();

private:
	
	void initVulkan();
	void mainLoop();
};