#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include "EngineUtility.hpp"
#include "DeviceUtils.hpp"

#include "Window.hpp"
#include "UniformBufferData.hpp"
#include "Vertex.hpp"
#include "Device.hpp"
#include "SwapChain.hpp"
#include "Pipeline.hpp"
#include "RenderPass.hpp"
#include "Description.hpp"
#include "Command.hpp"
#include "VertexBufferObject.hpp"
#include "IndexBufferObject.hpp"
#include "UniformBufferObject.hpp"
#include "UniformBufferOperator.hpp"
#include "ModelLoader.hpp"
#include "ImageLoader.hpp"
#include "Loader.hpp"
#include "TextureImageObject.hpp"
#include "DepthImageObject.hpp"
#include "ColorImageObject.hpp"
#include "FrameBufferObject.hpp"
#include "Mesh.hpp"
#include "Canvas.hpp"

//THE MAIN APPLICATION CLASS
class Application {

public:

	//Construction
	Application();
	~Application();

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

	//Loading
	ModelLoader myModelLoader;
	ImageLoader myImageLoader;
	Loader myLoader;

	//Window
	Window myWindow;
	
	//Utility
	EngineUtility myEngineUtility;

	//LOGICAL DEVICE OBJECTS
	std::unique_ptr<Device> myDevice;

	//SWAP CHAIN OBJECTS
	std::unique_ptr<SwapChain> mySwapChain;

	//GRAPHICS PIPELINE OBJECTS
	std::unique_ptr<Pipeline> myPipeline;
	std::unique_ptr<Canvas> myCanvas;

	//COMMAND OBJECTS
	Command myCommand;

	//BUFFER OBJECTS
	std::unique_ptr<Mesh> myMesh;

	//SYNCHRONISATION
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	//DRAWING
	uint32_t currentFrame = 0;

public:

	void run();

private:

	//DRAWING FUNCTIONS
	void drawFrame();
	
	//MAIN SETUP AND RUNTIME FUNCTIONS
	void initVulkan();
	void mainLoop();

	//OBJECT CREATION FUNCTIONS
	void createSyncObjects();

	//OBJECT CREATION SUPPORT FUNCTIONS
	void recreateSwapChain();
};