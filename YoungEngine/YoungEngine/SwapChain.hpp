#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <algorithm>

#include "DeviceUtils.hpp"
#include "SwapChainSupportDetails.hpp"
#include "Device.hpp"

class SwapChain {

public:

	SwapChain() = default;

	SwapChain(Device *inDevice, VkPhysicalDevice *physicalDevice, VkSurfaceKHR *surface, GLFWwindow *window);

	VkSwapchainKHR swapChain;

	std::vector<VkImageView> swapChainImageViews;

	std::vector<VkImage> swapChainImages;

	VkFormat swapChainImageFormat;

	VkExtent2D swapChainExtent;

	void createImageViews();

	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

private:

	Device *device;

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
	
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
	
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *window);
};