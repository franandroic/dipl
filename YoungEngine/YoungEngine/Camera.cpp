#include "Camera.hpp"

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