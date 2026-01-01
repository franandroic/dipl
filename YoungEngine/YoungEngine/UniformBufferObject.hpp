#pragma once

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

class UniformBufferObject {

	//STRUCTURE TO BE PASSED TO A VERTEX SHADER AS A UNIFORM BUFFER
	//CONTAINING THE MODEL. VIEW AND PROJECTION MATRICES

public:
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};