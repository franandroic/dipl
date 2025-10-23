#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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

const std::vector<const char *> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char *> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

//STRUCTURE CONTAINING INFORMATION ABOUT A QUEUE FAMILY USED BY PHYSICAL AND LOGICAL DEVICE AND SWAP CHAIN
struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

//STRUCTURE CONTAINING DETAILS NEEDED TO CREATE A SWAP CHAIN
struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

//THE MAIN APPLICATION CLASS
class Application {

public:

	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

private:

	//MAIN OBJECTS RELATED TO INSTANCE, PHYSICAL AND LOGICAL DEVICE AND WINDOW
	GLFWwindow *window;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;
	VkSurfaceKHR surface;
	VkQueue presentQueue;

	//SWAP CHAIN OBJECTS
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;

	//GRAPHICS PIPELINE OBJECTS
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	//BUFFERS
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkCommandBuffer commandBuffer;

	//COMMAND POOLS
	VkCommandPool commandPool;

public:

	void run();

private:
	
	//MAIN SETUP AND RUNTIME FUNCTIONS
	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanup();

	//OBJECT CREATION FUNCTIONS
	void createInstance();
	void setupDebugMessenger();
	void createSurface();
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSwapChain();
	void createImageViews();
	void createRenderPass();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createCommandPool();
	void createCommandBuffer();

	//INSTANCE AND DEBUG MESSENGER SUPPORT FUNCTIONS
	std::vector<const char *> getRequiredExtensions(bool verbose);
	bool checkValidationLayerSupport(bool verbose);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

	//PHYSICAL AND LOGICAL DEVICE SUPPORT FUNCTIONS
	bool isDeviceSuitable(VkPhysicalDevice device, bool verbose);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device, bool verbose);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	//SWAP CHAIN SUPPORT FUNCTIONS
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

	//GRAPHICS PIPELINE SUPPORT FUNCTIONS
	VkShaderModule createShaderModule(const std::vector<char> &code);

	//COMMAND RECORDING FUNCTION
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

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

	//FUNCTION TO READ SHADER CODE COMPILED TO SPIR-V
	static std::vector<char> readFile(const std::string &filename) {

		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

};