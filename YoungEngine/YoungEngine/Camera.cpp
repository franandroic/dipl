#include "Camera.hpp"

Camera::Camera(Device &inDevice, size_t framesInFlight) : device(inDevice) {

	//The data changes every frame, so each frame in flight gets its own uniform buffer
	//(the GPU might still be reading the one of the previous frame).

	UBOs.reserve(framesInFlight);
	descriptorSets.reserve(framesInFlight);

	for (size_t i = 0; i < framesInFlight; i++) {
		UBOs.push_back(std::make_unique<UniformBufferObject>(
			device,
			sizeof(UniformBlock),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		));
		UBOs[i]->createBuffer();

		descriptorSets.push_back(std::make_unique<DescriptorSet>(device, std::vector<VkDescriptorSetLayoutBinding>{ makeBinding() }));
		descriptorSets[i]->writeUniformBuffer(BUFFER_BINDING, *UBOs[i]);
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

void Camera::update(size_t frame) {

	UniformBlock block = getUniformBlock();
	UBOs[frame]->updateBuffer(&block, sizeof(block));
}

VkDescriptorSetLayoutBinding Camera::makeBinding() {

	VkDescriptorSetLayoutBinding binding{};
	binding.binding = BUFFER_BINDING;
	binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	binding.descriptorCount = 1;
	binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	binding.pImmutableSamplers = nullptr;

	return binding;
}