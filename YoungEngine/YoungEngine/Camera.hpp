#pragma once
#include "GLFWConfig.hpp"
#include "GLMConfig.hpp"

#include <cstdint>

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

	//Construction
	Camera() = default;

	//Member variables
	glm::vec3 position{ 0.0f, -2.0f, 1.0f };
	glm::vec3 target{ 0.0f, 0.0f, 0.0f };
	glm::vec3 up{ 0.0f, 0.0f, 1.0f };
	float fovDegrees = 45.0f;
	float nearPlane = 0.1f;
	float farPlane = 10.0f;

	void setViewportExtent(uint32_t width, uint32_t height);

	UniformBlock getUniformBlock() const;

private:

	float aspectRatio = 1.0f;
};