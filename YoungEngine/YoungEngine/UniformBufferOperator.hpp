#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#include "UniformBufferData.hpp"

class UniformBufferOperator {

	//TODO: make this an abstract class and add concrete implementations to be used in UniformBufferObject

public:

	UniformBufferOperator() = default;

	UniformBufferOperator(float inAspectRatio) {
		aspectRatio = inAspectRatio;
	}

	void operate(UniformBufferData *data) {

		//Updating the contents of a uniform buffer.
		//This is called every frame and it updates the transformation matrices.
		//Updating the contents of the memory mapped to the GPU passes values to shaders.

		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		data->model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		data->view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		data->proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 10.0f);
		data->proj[1][1] *= -1;
	}

private:

	float aspectRatio;

};