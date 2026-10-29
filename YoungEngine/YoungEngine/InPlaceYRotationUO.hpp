#pragma once
#include "GLFWConfig.hpp"
#include "GLMConfig.hpp"

#include <cmath>
#include <stdexcept>

#include <glm/gtc/constants.hpp>

#include "UniformOperator.hpp"
#include "SimpleModelUD.hpp"

//Rotates the model around its own Y axis at a constant speed, staying at the origin.
//Works with SimpleModelUD.
class InPlaceYRotationUO : public UniformOperator {

public:

	//Construction
	explicit InPlaceYRotationUO(float inDegreesPerSecond) : degreesPerSecond(inDegreesPerSecond) {}

	void operate(UniformData &data, float dt) override {

		SimpleModelUD *modelData = dynamic_cast<SimpleModelUD *>(&data);
		if (!modelData) {
			throw std::invalid_argument("InPlaceYRotationUOp can only operate on SimpleModelUD!");
		}

		angle = std::fmod(angle + dt * glm::radians(degreesPerSecond), glm::two_pi<float>());

		modelData->pushConstants.model =
			glm::translate(glm::mat4(1.0f), modelData->position) *
			glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
	}

private:

	float degreesPerSecond;
	float angle = 0.0f;
};