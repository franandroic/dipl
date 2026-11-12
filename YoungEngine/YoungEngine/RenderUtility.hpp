#pragma once
#include "GLFWConfig.hpp"

#include <vector>
#include <algorithm>
#include <functional>

class RenderUtility {

public:

	static bool sameBindings(const std::vector<VkDescriptorSetLayoutBinding> &a, const std::vector<VkDescriptorSetLayoutBinding> &b) {

		if (a.size() != b.size()) return false;

		for (size_t i = 0; i < a.size(); i++) {
			if (a[i].binding != b[i].binding
				|| a[i].descriptorType != b[i].descriptorType
				|| a[i].descriptorCount != b[i].descriptorCount
				|| a[i].stageFlags != b[i].stageFlags) {
				return false;
			}
		}

		return true;
	}

	static bool samePushConstantRange(const VkPushConstantRange &a, const VkPushConstantRange &b) {

		return a.stageFlags == b.stageFlags && a.offset == b.offset && a.size == b.size;
	}

	static void setViewportAndScissor(VkCommandBuffer commandBuffer, VkExtent2D extent) {

		//Viewport and scissor are dynamic state of every pipeline, so they're set once per rendering scope,
		//to cover the whole extent of what's rendered into.

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(extent.width);
		viewport.height = static_cast<float>(extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = extent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}
};