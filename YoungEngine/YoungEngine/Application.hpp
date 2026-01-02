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

#include <stdexcept>
#include <vector>
#include <iostream>
#include <string>
#include <optional>
#include <set>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <fstream>
#include <array>
#include <chrono>
#include <unordered_map>

#include "DeviceData.hpp"
#include "DeviceUtils.hpp"

#include "QueueFamilyIndices.hpp"
#include "SwapChainSupportDetails.hpp"
#include "UniformBufferObject.hpp"
#include "Vertex.hpp"
#include "Device.hpp"
#include "SwapChain.hpp"
#include "Pipeline.hpp"
#include "RenderPass.hpp"
#include "Description.hpp"

//THE MAIN APPLICATION CLASS
class Application {

public:

	Application() = default;

	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	const std::string MODEL_PATH = "models/viking_room.obj";
	const std::string TEXTURE_PATH = "textures/viking_room.png";

	//VERTICES
	std::vector<Vertex> vertices;

	//INDICES
	std::vector<uint32_t> indices;

private:

	//MAIN OBJECTS RELATED TO INSTANCE, PHYSICAL DEVICE AND WINDOW
	GLFWwindow *window;
	VkInstance instance;
	VkSurfaceKHR surface;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	//LOGICAL DEVICE OBJECTS
	Device myDevice;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	//SWAP CHAIN OBJECTS
	SwapChain mySwapChain;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;

	//GRAPHICS PIPELINE OBJECTS
	Pipeline myPipeline;
	RenderPass myRenderPass;
	Description myDescription;
	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	std::vector<VkDescriptorSet> descriptorSets;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	//BUFFERS
	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkCommandBuffer> commandBuffers;
	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	std::vector<VkBuffer> uniformBuffers;

	//OBJECT POOLS
	VkCommandPool commandPool;
	VkDescriptorPool descriptorPool;

	//SYNCHRONISATION
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	//DRAWING
	uint32_t currentFrame = 0;
	bool framebufferResized = false;

	//MEMORY
	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory indexBufferMemory;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void *> uniformBuffersMapped;
	VkDeviceMemory textureImageMemory;
	VkDeviceMemory depthImageMemory;
	VkDeviceMemory colorImageMemory;

	//IMAGE/TEXTURE OBJECTS
	VkImage textureImage;
	VkImageView textureImageView;
	VkSampler textureSampler;
	VkImage depthImage;
	VkImageView depthImageView;
	uint32_t mipLevels;
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	VkImage colorImage;
	VkImageView colorImageView;

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
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapChain();
	void createImageViews();
	void createRenderPass();
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createDepthResources();
	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();
	void createVertexBuffer();
	void createIndexBuffer();
	void createUniformBuffers();
	void createDescriptorPool();
	void createDescriptorSets();
	void createCommandBuffers();
	void createSyncObjects();

	//MODEL LOADING FUNCTIONS
	void loadModel();

	//OBJECT CREATION SUPPORT FUNCTIONS
	void recreateSwapChain();
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

	//SUPPORT DRAWING FUNCTIONS
	void updateUniformBuffer(uint32_t currentImage);

	//INSTANCE AND DEBUG MESSENGER SUPPORT FUNCTIONS
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

	//COMMAND RECORDING FUNCTIONS
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	//BUFFER SUPPORT FUNCTIONS
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	//IMAGE SUPPORT FUNCTIONS
	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageAspectFlagBits aspect, uint32_t mipLevels, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	bool hasStencilComponent(VkFormat format);
	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
	VkSampleCountFlagBits getMaxUsableSampleCount();
	void createColorResources();

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