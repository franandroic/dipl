#pragma once
#include "GLFWConfig.hpp"
#include "GLMConfig.hpp"

#include <cstdint>
#include <memory>
#include <vector>

#include "Context.hpp"
#include "Device.hpp"
#include "DescriptorSet.hpp"
#include "UniformBufferObject.hpp"

//Describes the point of view the scene is rendered from.
class Camera {

public:

	//The data the camera passes to the shaders, laid out to match the uniform block in the vertex shader.
	//It changes once per frame and is shared by every object drawn in it.
	struct UniformBlock {
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
		alignas(16) glm::vec3 position;
	};

	//Member variables
	glm::vec3 position{ 0.0f, 0.0f, 0.0f };
	glm::vec3 target{ 0.0f, 0.0f, 1.0f };
	glm::vec3 up = Context::GLOBAL_UP;
	float fovDegrees = 45.0f;
	float nearPlane = 0.1f;
	float farPlane = 10.0f;

	void setViewportExtent(uint32_t width, uint32_t height);
	float getAspectRatio() const { return aspectRatio; }
	UniformBlock getUniformBlock() const;

	void move(glm::vec3 direction);
	void turn(glm::vec3 direction);

private:

	float aspectRatio = 1.0f;
	float maxTurnAngle = 0.995f;

	float moveSpeed = 0.5f;
	float turnSpeed = 0.002f;
};