#pragma once
#include "GLFWConfig.hpp"

#include <vector>

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

const int MAX_FRAMES_IN_FLIGHT = 2;

const uint32_t MAX_DESCRIPTOR_SETS = 1000;
const uint32_t MAX_UNIFORM_BUFFER_DESCRIPTORS = 1000;
const uint32_t MAX_COMBINED_IMAGE_SAMPLER_DESCRIPTORS = 1000;