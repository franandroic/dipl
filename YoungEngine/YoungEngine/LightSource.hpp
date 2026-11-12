#pragma once
#include "GLMConfig.hpp"

class LightSource {

public:

	//Member variables
	glm::vec3 position{ 0.0f, 0.0f, 3.0f };
	float intensity = 1.0f;
};