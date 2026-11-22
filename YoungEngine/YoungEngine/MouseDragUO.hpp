#pragma once
#include "GLFWConfig.hpp"
#include "GLMConfig.hpp"

#include <stdexcept>

#include "UniformOperator.hpp"
#include "SimpleModelUD.hpp"
#include "SimpleLitUD.hpp"
#include "LightSource.hpp"

class MouseDragUO : public UniformOperator {

public:

	//Construction
	explicit MouseDragUO(const LightSource *inLight = nullptr) : light(inLight) {}

	void drag(float angle, glm::vec3 axis) {

		if (glm::length(axis) < 1e-6f) return;

		orientation = glm::rotate(glm::mat4(1.0f), angle, glm::normalize(axis)) * orientation;
	}

	void operate(UniformData &data, float /*dt*/) override {

		glm::vec3 *position = nullptr;
		glm::mat4 *model = nullptr;

		if (SimpleModelUD *modelData = dynamic_cast<SimpleModelUD *>(&data)) {

			if (light) {
				throw std::invalid_argument("MouseDragUO was given a light, but SimpleModelUD has no light block!");
			}

			position = &modelData->position;
			model = &modelData->pushConstants.model;

		} else if (SimpleLitUD *litData = dynamic_cast<SimpleLitUD *>(&data)) {

			if (light) litData->setLight(*light);

			position = &litData->position;
			model = &litData->pushConstants.model;

		} else {

			throw std::invalid_argument("MouseDragUO can only operate on SimpleModelUD or SimpleLitUD!");
		}

		*model = glm::translate(glm::mat4(1.0f), *position) * orientation;
	}

private:

	glm::mat4 orientation{ 1.0f };
	const LightSource *light;
};