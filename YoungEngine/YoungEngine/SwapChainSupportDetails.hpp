#pragma once

#include <vector>

class SwapChainSupportDetails {

	//STRUCTURE CONTAINING DETAILS NEEDED TO CREATE A SWAP CHAIN

public:
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;

};
