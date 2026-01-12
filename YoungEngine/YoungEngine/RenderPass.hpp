#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <array>

#include "DeviceUtils.hpp"

#include "Device.hpp"
#include "SwapChain.hpp"

class RenderPass {

public:

	RenderPass() = default;

	RenderPass(Device *inDevice, SwapChain *swapChain);

	VkRenderPass renderPass;

private:

	Device *device;

};