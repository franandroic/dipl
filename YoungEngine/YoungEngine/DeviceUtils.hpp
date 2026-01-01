#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
#include <string>
#include <set>

#include "DeviceData.hpp"
#include "QueueFamilyIndices.hpp"
#include "SwapChainSupportDetails.hpp"

class DeviceUtils {

public:
	static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {

		//We're interested in a queue family to serve for graphics calculation purposes
		//and one to serve for screen presentation purposes. We list all available queue
		//families on the device and check whether one supports one or both of desired purposes.
		//We note the indices of the queue/s in the list and save them into our custom struct.

		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto &queueFamily : queueFamilies) {

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			if (presentSupport) indices.presentFamily = i;

			if (indices.isComplete()) break;

			i++;
		}

		return indices;
	}

	static std::vector<const char *> getRequiredExtensions(bool verbose) {

		//The entities needing extensions are: GLFW - window creation and management,
		//									   Validation Layers - debugging
		//We check for required and available extensions and return a list of
		//required extensions to enable.

		uint32_t glfwExtensionCount = 0;
		const char **glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		if (verbose) {
			std::cout << "Required extensions (GLFW): " << std::endl;
			for (int i = 0; i < glfwExtensionCount; i++) std::cout << '\t' << glfwExtensions[i] << std::endl;
			std::cout << "Required extensions (validation): " << std::endl;
			std::cout << '\t' << VK_EXT_DEBUG_UTILS_EXTENSION_NAME << std::endl;
			std::cout << "Available extensions: " << std::endl;
			for (const auto &extension : extensions) std::cout << '\t' << extension.extensionName << std::endl;
		}

		std::vector<const char *> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) {
			requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return requiredExtensions;
	}

	static bool checkValidationLayerSupport(bool verbose) {

		//Checking for wanted validation layers availability and returning information on whether they are.

		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		if (verbose) {
			std::cout << "Required validation layers: " << std::endl;
			for (const auto &vLayer : validationLayers) std::cout << '\t' << vLayer << std::endl;
			std::cout << "Available validation layers: " << std::endl;
			for (const auto &vLayer : availableLayers) std::cout << '\t' << vLayer.layerName << std::endl;
		}

		for (const char *layerName : validationLayers) {

			bool layerFound = false;

			for (const auto &layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) return false;
		}

		return true;
	}

	static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, bool verbose) {

		//For a physical device to be suitable we need to have information on desired queue families,
		//the desired extensions need to be supported and the swap chain needs to have the desired
		//features available.

		//TODO: More verbose print of properties and features
		//TODO: Better selection process among suitable devices

		if (verbose) {
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);

			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

			std::cout << "Device properties: " << std::endl;
			std::cout << '\t' << deviceProperties.deviceName << std::endl;
			std::cout << '\t' << deviceProperties.deviceID << std::endl;
			std::cout << '\t' << deviceProperties.deviceType << std::endl;
		}

		QueueFamilyIndices indices = DeviceUtils::findQueueFamilies(device, surface);

		bool extensionsSupported = DeviceUtils::checkDeviceExtensionSupport(device, true);

		bool swapChainAdequate = false;
		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = DeviceUtils::querySwapChainSupport(device, surface);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}

	static bool checkDeviceExtensionSupport(VkPhysicalDevice device, bool verbose) {

		//Checking if all the device extensions defined (in the global vector in the .hpp file)
		//as required for the application are present in the list of available device extensions.

		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		if (verbose) {
			std::cout << "Required device extensions: " << std::endl;
			for (std::string extension : requiredExtensions) std::cout << '\t' << extension << std::endl;
			std::cout << "Available device extensions: " << std::endl;
			for (const auto &extension : availableExtensions) std::cout << '\t' << extension.extensionName << std::endl;
		}

		for (const auto &extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {

		//Querying for all the available features of the swap chain and adding them to a custom struct.

		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}
};