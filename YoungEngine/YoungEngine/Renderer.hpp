#pragma once
#include "GLFWConfig.hpp"

#include <memory>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <functional>

#include "ImageUtility.hpp"

#include "Device.hpp"
#include "SwapChain.hpp"
#include "Window.hpp"
#include "Synchronizer.hpp"
#include "Command.hpp"
#include "RenderTarget.hpp"
#include "PipelineCache.hpp"
#include "PipelineDescription.hpp"
#include "DescriptorSet.hpp"
#include "UniformBufferObject.hpp"
#include "Camera.hpp"
#include "Scene.hpp"
#include "DrawItem.hpp"
#include "Material.hpp"

//Orchestrates a frame: synchronization, acquiring and presenting swap chain images, the command buffer,
//the attachments and the rendering scope.
class Renderer {

public:

	//Constructors
	Renderer(
		Window &inWindow,
		Scene &inScene,
		size_t framesInFlight
	);
	~Renderer() = default;

	//Copy and Move constructors
	Renderer(const Renderer &) = delete;
	Renderer &operator=(const Renderer &) = delete;
	Renderer(Renderer &&) = delete;
	Renderer &operator=(Renderer &&) = delete;

	void setScene(Scene &inScene) { scene = &inScene; }
	Scene &getScene() const { return *scene; }
	const RenderTarget &getRenderTarget() const { return renderTarget; }

	void render(bool externalResizeRequested);

protected:

	Window &window;
	Scene *scene;

	RenderTarget renderTarget;
	Synchronizer sync;
	Command command;

	std::vector<std::unique_ptr<UniformBufferObject>> frameUBOs;
	std::vector<std::unique_ptr<DescriptorSet>> frameSets;
	size_t currentFrame = 0;

	//Sorted so that the items that render into textures come first, and the items drawn into the screen
	//start at firstScreenItem. Within both, the items of a material are together, in the order of its passes.
	std::vector<DrawItem> drawItems;
	size_t firstScreenItem = 0;

	void updateFrame(size_t frame);
	void recordFrame(VkCommandBuffer commandBuffer, uint32_t imageIndex, size_t frame);
	void recordTexturePasses(VkCommandBuffer commandBuffer, size_t frame);
	void recordDrawItems(VkCommandBuffer commandBuffer, size_t frame, size_t first, size_t last);
	void submitFrame(size_t frame, VkSemaphore imageAvailable, VkSemaphore renderFinished, VkFence inFlight);
	void recreateSwapChain();

	//The texture an item renders into, null for the screen
	static const Texture *targetOf(const DrawItem &item);
	static void setViewportAndScissor(VkCommandBuffer commandBuffer, VkExtent2D extent);
};