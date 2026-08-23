#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "DeviceUtils.hpp"

//Contains Vulkan instance and basic engine utility,
//such as validation layers and debug messenger
class EngineUtility {

public:

	//Construction
	EngineUtility();
	~EngineUtility();

	//Copy and Move constructors
	EngineUtility(const EngineUtility &) = delete;
	EngineUtility &operator=(const EngineUtility &) = delete;
	EngineUtility(EngineUtility &&) = delete;
	EngineUtility &operator=(EngineUtility &&) = delete;

	//Member variables
	VkInstance instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

private:

	void createInstance();
	void setupDebugMessenger();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

	//DEBUG CALLBACK FUNCTION
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
		void *pUserData
	);

	//FUNCTIONS TO EXPLICITLY LOAD EXTENSION FUNCTIONS
	static VkResult CreateDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
		const VkAllocationCallbacks *pAllocator,
		VkDebugUtilsMessengerEXT *pDebugMessenger
	);

	static void DestroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks *pAllocator
	);
};