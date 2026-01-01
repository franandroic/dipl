#pragma once

#include <optional>

class QueueFamilyIndices {

	//STRUCTURE CONTAINING INFORMATION ABOUT A QUEUE FAMILY USED BY PHYSICAL AND LOGICAL DEVICE AND SWAP CHAIN

public:
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}

};