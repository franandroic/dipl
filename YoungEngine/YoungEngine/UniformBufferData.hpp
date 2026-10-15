#pragma once
#include "GLMConfig.hpp"

class UniformBufferData {

	//STRUCTURE TO BE PASSED TO A VERTEX SHADER AS A UNIFORM BUFFER
	//CONTAINING THE MODEL. VIEW AND PROJECTION MATRICES

	//TODO: make this an abstract class and add concrete implementations to use in UniformBufferObject

public:
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};