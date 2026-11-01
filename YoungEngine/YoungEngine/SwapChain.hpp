#pragma once
#include "GLFWConfig.hpp"

#include <vector>
#include <algorithm>
#include <assert.h>

#include "DeviceUtils.hpp"
#include "SwapChainSupportDetails.hpp"
#include "Device.hpp"

//Holds swapChainImages, which are final render targets a Renderer renders into.
//Handles their creation and presentation into present queue.
//Also handles reconstruction in the case of window resize.
class SwapChain {

public:

	//Construction
	SwapChain(GLFWwindow *window);
	~SwapChain();

	//Copy and Move constructors
	SwapChain(const SwapChain &) = delete;
	SwapChain &operator=(const SwapChain &) = delete;
	SwapChain(SwapChain &&) = delete;
	SwapChain &operator=(SwapChain &&) = delete;

	//Local enum
	enum class AcquireResult {
		Success,
		Suboptimal,
		OutOfDate
	};

	//Global access
	static inline SwapChain *active = nullptr;

	//Member variables
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