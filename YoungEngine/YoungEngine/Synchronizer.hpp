#pragma once
#include "GLFWConfig.hpp"

#include <vector>
#include <stdexcept>

#include "Device.hpp"

//Holds Vulkan's sync objects and is called by the renderer inside the draw function
class Synchronizer {

public:

	//Construction
	Synchronizer(Device &inDevice, size_t framesInFlight);
	~Synchronizer();

	//Copy and Move constructors
	Synchronizer(const Synchronizer &) = delete;
	Synchronizer &operator=(const Synchronizer &) = delete;
	Synchronizer(Synchronizer &&) = delete;
	Synchronizer &operator=(Synchronizer &&) = delete;

	void stop(size_t frame) const;
	void go(size_t frame) const;

	VkSemaphore imageAvailable(size_t frame) const;
	VkSemaphore renderFinished(size_t frame) const;
	VkFence inFlightFence(size_t frame) const;

	size_t count() const;

private:

	Device &device;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
};