#include "Camera.hpp"

Camera::Camera(Device &inDevice, size_t framesInFlight) : device(inDevice) {

	//The data changes every frame, so each frame in flight gets its own uniform buffer
	//(the GPU might still be reading the one of the previous frame).

	UBOs.reserve(framesInFlight);

	for (size_t i = 0; i < framesInFlight; i++) {
		UBOs.push_back(std::make_unique<UniformBufferObject>(
			device,
			sizeof(UniformBlock),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		));
		UBOs[i]->createBuffer();
	}
}

void Camera::setViewportExtent(uint32_t width, uint32_t height) {

	//A minimized window has a zero extent, which would produce an invalid aspect ratio.

	if (width == 0 || height == 0) return;

	aspectRatio = width / static_cast<float>(height);
}

Camera::UniformBlock Camera::getUniformBlock() const {

	UniformBlock block;

	block.view = glm::lookAt(position, target, up);
	block.proj = glm::perspective(glm::radians(fovDegrees), aspectRatio, nearPlane, farPlane);
	block.position = position;

	//Y-axis conversion from the OpenGL standard
	block.proj[1][1] *= -1;

	return block;
}

std::vector<VkDescriptorSetLayoutBinding> Camera::bindings() {

	return {
		Description::makeBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
	};
}

void Camera::createDescriptorSets(const Description &description) {

	//The buffers stay the same for the whole lifetime of the camera, so the sets are written only once.

	descriptorSets.clear();
	descriptorSets.reserve(UBOs.size());

	for (size_t i = 0; i < UBOs.size(); i++) {
		descriptorSets.push_back(std::make_unique<DescriptorSet>(device, description));
		descriptorSets[i]->writeUniformBuffer(0, *UBOs[i]);
	}
}

void Camera::update(size_t frame) {

	UniformBlock block = getUniformBlock();
	UBOs[frame]->updateBuffer(&block, sizeof(block));
}