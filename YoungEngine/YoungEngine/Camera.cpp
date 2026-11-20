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

void Camera::move(glm::vec3 direction) {

	glm::vec3 forward = target - position;
	glm::vec3 right = glm::cross(forward, up);
	glm::vec3 localUp = glm::vec3(0.0f);

	if (glm::length(forward) < 1e-6f || glm::length(right) < 1e-6f) return;

	forward = glm::normalize(forward);
	right = glm::normalize(right);
	localUp = glm::cross(right, forward);

	float alongRight = glm::dot(direction, Context::CAMERA_RIGHT);
	float alongForward = glm::dot(direction, Context::CAMERA_FORWARD);
	float alongUp = glm::dot(direction, Context::CAMERA_UP);

	glm::vec3 offset = right * alongRight + forward * alongForward + localUp * alongUp;

	position += offset * moveSpeed;
	target += offset * moveSpeed;
}

void Camera::turn(glm::vec3 direction) {

	glm::vec3 forward = target - position;
	glm::vec3 right = glm::cross(forward, up);
	glm::vec3 localUp = glm::vec3(0.0f);

	float distance = glm::length(forward);
	if (distance < 1e-6f || glm::length(right) < 1e-6f) return;

	forward /= distance;
	right = glm::normalize(right);
	localUp = glm::cross(right, forward);

	float alongRight = glm::dot(direction, Context::CAMERA_RIGHT) * turnSpeed;
	float alongForward = glm::dot(direction, Context::CAMERA_FORWARD) * turnSpeed;
	float alongUp = glm::dot(direction, Context::CAMERA_UP) * turnSpeed;

	glm::vec3 turned = glm::normalize(
		forward +
		right * alongRight +
		forward * alongForward +
		localUp * alongUp
	);

	if (std::abs(glm::dot(turned, glm::normalize(up))) > maxTurnAngle) {
		turned = glm::normalize(forward + right * alongRight + forward * alongForward);
	}

	target = position + turned * distance;
}