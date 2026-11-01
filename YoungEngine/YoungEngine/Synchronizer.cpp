#include "Synchronizer.hpp"

Synchronizer::Synchronizer(size_t framesInFlight) {

	imageAvailableSemaphores.resize(framesInFlight, VK_NULL_HANDLE);
	renderFinishedSemaphores.resize(framesInFlight, VK_NULL_HANDLE);
	inFlightFences.resize(framesInFlight, VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < framesInFlight; i++) {
		if (vkCreateSemaphore(Device::active->logical, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(Device::active->logical, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(Device::active->logical, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			destroy();
			throw std::runtime_error("Failed to create sync objects!");
		}
	}
}

Synchronizer::~Synchronizer() {

	destroy();
}

void Synchronizer::destroy() {

	for (size_t i = 0; i < inFlightFences.size(); i++) {
		vkDestroySemaphore(Device::active->logical, imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(Device::active->logical, renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(Device::active->logical, inFlightFences[i], nullptr);
	}

	imageAvailableSemaphores.clear();
	renderFinishedSemaphores.clear();
	inFlightFences.clear();
}

void Synchronizer::stop(size_t frame) const {
	vkWaitForFences(Device::active->logical, 1, &inFlightFences[frame], VK_TRUE, UINT64_MAX);
}

void Synchronizer::go(size_t frame) const {
	vkResetFences(Device::active->logical, 1, &inFlightFences[frame]);
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