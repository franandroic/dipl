#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

class UniformBufferData {

	//STRUCTURE TO BE PASSED TO A VERTEX SHADER AS A UNIFORM BUFFER
	//CONTAINING THE MODEL. VIEW AND PROJECTION MATRICES

	//TODO: make this an abstract class and add concrete implementations to use in UniformBufferObject

public:
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};