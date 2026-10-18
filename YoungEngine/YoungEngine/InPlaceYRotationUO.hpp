#pragma once
#include "GLFWConfig.hpp"
#include "GLMConfig.hpp"

#include <chrono>
#include <stdexcept>

#include "UniformOperator.hpp"
#include "SimpleModelUD.hpp"

//Rotates the model around its own Y axis at a constant speed, staying at the origin.
//Works with SimpleModelUD.
class InPlaceYRotationUO : public UniformOperator {

public:

	//Construction
	explicit InPlaceYRotationUO(float inDegreesPerSecond)
		: degreesPerSecond(inDegreesPerSecond),
		startTime(std::chrono::steady_clock::now()) {}

	void operate(UniformData &data) override {

		SimpleModelUD *modelData = dynamic_cast<SimpleModelUD *>(&data);
		if (!modelData) {
			throw std::invalid_argument("InPlaceYRotationUOp can only operate on SimpleModelUD!");
		}

		auto currentTime = std::chrono::steady_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		modelData->pushConstants.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(degreesPerSecond), glm::vec3(0.0f, 0.0f, 1.0f));
	}

private:

	float degreesPerSecond;
	std::chrono::steady_clock::time_point startTime;
};