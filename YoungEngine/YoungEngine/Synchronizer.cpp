#include "Synchronizer.hpp"

Synchronizer::Synchronizer(Device &inDevice, size_t framesInFlight) : device(inDevice) {

	imageAvailableSemaphores.resize(framesInFlight);
	renderFinishedSemaphores.resize(framesInFlight);
	inFlightFences.resize(framesInFlight);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < framesInFlight; i++) {
		if (vkCreateSemaphore(device.logical, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device.logical, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device.logical, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create sync objects!");
		}
	}
}

Synchronizer::~Synchronizer() {

	for (size_t i = 0; i < inFlightFences.size(); i++) {
		vkDestroySemaphore(device.logical, imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(device.logical, renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(device.logical, inFlightFences[i], nullptr);
	}
}

void Synchronizer::stop(size_t frame) const {
	vkWaitForFences(device.logical, 1, &inFlightFences[frame], VK_TRUE, UINT64_MAX);
}

void Synchronizer::go(size_t frame) const {
	vkResetFences(device.logical, 1, &inFlightFences[frame]);
}

VkSemaphore Synchronizer::imageAvailable(size_t frame) const {
	return imageAvailableSemaphores[frame];
}

VkSemaphore Synchronizer::renderFinished(size_t frame) const {
	return renderFinishedSemaphores[frame];
}

VkFence Synchronizer::inFlightFence(size_t frame) const {
	return inFlightFences[frame];
}

size_t Synchronizer::count() const {
	return inFlightFences.size();
}