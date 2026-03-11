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

#include "DeviceUtils.hpp"

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

	Application() = default;

	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	const std::string MODEL_PATH = "models/suzanne.obj";
	const std::string TEXTURE_PATH = "textures/Discr2DTex5.png";

	//VERTICES
	std::vector<Vertex> vertices;

	//INDICES
	std::vector<uint32_t> indices;

private:

	//LOADERS
	ModelLoader myModelLoader;
	ImageLoader myImageLoader;
	Loader myLoader;

	//MAIN OBJECTS RELATED TO INSTANCE, PHYSICAL DEVICE AND WINDOW
	GLFWwindow *window;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;

	//LOGICAL DEVICE OBJECTS
	Device myDevice;

	//SWAP CHAIN OBJECTS
	SwapChain mySwapChain;

	//GRAPHICS PIPELINE OBJECTS
	Pipeline myPipeline;
	std::unique_ptr<Canvas> myCanvas;

	//COMMAND OBJECTS
	Command myCommand;

	//BUFFER OBJECTS
	Mesh myMesh;

	//SYNCHRONISATION
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	//DRAWING
	uint32_t currentFrame = 0;
	bool framebufferResized = false;

public:

	void run();

private:

	//DRAWING FUNCTIONS
	void drawFrame();
	
	//MAIN SETUP AND RUNTIME FUNCTIONS
	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanup();
	void cleanupSwapChain();

	//OBJECT CREATION FUNCTIONS
	void createInstance();
	void setupDebugMessenger();
	void createSyncObjects();

	//OBJECT CREATION SUPPORT FUNCTIONS
	void recreateSwapChain();

	//INSTANCE AND DEBUG MESSENGER SUPPORT FUNCTIONS
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

	//DEBUG CALLBACK FUNCTION
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
		void *pUserData
		) {
			std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
			return VK_FALSE;
	}

	//WINDOW RESIZE CALLBACK FUNCTION
	static void framebufferResizeCallback(GLFWwindow *window, int width, int height) {
		auto app = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}

	//FUNCTIONS TO EXPLICITLY LOAD EXTENSION FUNCTIONS
	static VkResult CreateDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
		const VkAllocationCallbacks *pAllocator,
		VkDebugUtilsMessengerEXT *pDebugMessenger
		) {
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
			if (func != nullptr) {
				return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
			} else {
				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
	}

	static void DestroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks *pAllocator
		) {
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
			if (func != nullptr) {
				func(instance, debugMessenger, pAllocator);
			}
	}

};