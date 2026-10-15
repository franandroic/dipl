#pragma once
#include "GLFWConfig.hpp"

#include <vector>
#include <algorithm>

#include "DeviceUtils.hpp"
#include "SwapChainSupportDetails.hpp"
#include "Device.hpp"

//Wrapper for Vulkan SwapChain
class SwapChain {

public:

	//Construction
	SwapChain(Device &inDevice, GLFWwindow *window);
	~SwapChain();

	//Copy and Move constructors
	SwapChain(const SwapChain &) = delete;
	SwapChain &operator=(const SwapChain &) = delete;
	SwapChain(SwapChain &&) = delete;
	SwapChain &operator=(SwapChain &&) = delete;

	//Local enums
	enum class AcquireResult {
		Success,
		Suboptimal,
		OutOfDate
	};

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

	AcquireResult acquireNextImage(VkSemaphore signalSemaphore, uint32_t &outImageIndex);
	bool present(VkSemaphore waitSemaphore, uint32_t imageIndex);

private:

	void create(GLFWwindow *window);
	void destroy();

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *window);
};