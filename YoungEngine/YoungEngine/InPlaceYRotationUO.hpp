#pragma once
#include "GLFWConfig.hpp"
#include "GLMConfig.hpp"

#include <cmath>
#include <stdexcept>

#include <glm/gtc/constants.hpp>

#include "Context.hpp"
#include "UniformOperator.hpp"
#include "SimpleModelUD.hpp"
#include "SimpleLitUD.hpp"
#include "LightSource.hpp"

//Rotates the model around its own Y axis at a constant speed, staying at the origin.
//Works with SimpleModelUD.
class InPlaceYRotationUO : public UniformOperator {

public:

	//Construction
	explicit InPlaceYRotationUO(float inDegreesPerSecond, const LightSource *inLight = nullptr)
		: degreesPerSecond(inDegreesPerSecond), light(inLight) {}

	void operate(UniformData &data, float dt) override {

		glm::vec3 *position = nullptr;
		glm::mat4 *model = nullptr;

		if (SimpleModelUD *modelData = dynamic_cast<SimpleModelUD *>(&data)) {

			if (light) {
				throw std::invalid_argument("InPlaceYRotationUO was given a light, but SimpleModelUD has no light block!");
			}

			position = &modelData->position;
			model = &modelData->pushConstants.model;

		} else if (SimpleLitUD *litData = dynamic_cast<SimpleLitUD *>(&data)) {

			if (light) litData->setLight(*light);

			position = &litData->position;
			model = &litData->pushConstants.model;

		} else {

			throw std::invalid_argument("InPlaceYRotationUO can only operate on SimpleModelUD or SimpleLitUD!");
		}

		angle = std::fmod(angle + dt * glm::radians(degreesPerSecond), glm::two_pi<float>());

		*model =
			glm::translate(glm::mat4(1.0f), *position) *
			glm::rotate(glm::mat4(1.0f), angle, Context::GLOBAL_UP);
	}

private:

	float degreesPerSecond;
	float angle = 0.0f;

	const LightSource *light;
};