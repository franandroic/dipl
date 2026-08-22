#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <algorithm>

#include "DeviceUtils.hpp"
#include "SwapChainSupportDetails.hpp"
#include "Device.hpp"

//Wrapper for Vulkan SwapChain
class SwapChain {

public:

	//Constructor
	SwapChain(Device &inDevice, GLFWwindow *window);

	//Copy and Move constructors
	SwapChain(const SwapChain &) = delete;
	SwapChain &operator=(const SwapChain &) = delete;
	SwapChain(SwapChain &&) = delete;
	SwapChain &operator=(SwapChain &&) = delete;

	//Member variables
	Device &device;
	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat{};
	VkExtent2D swapChainExtent{};

	void createImageViews();
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

	void recreate(GLFWwindow *window);

private:

	void create(GLFWwindow *window);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *window);
};