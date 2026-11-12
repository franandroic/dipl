#pragma once
#include "GLFWConfig.hpp"
#include "GLMConfig.hpp"

#include <stdexcept>

#include "UniformOperator.hpp"
#include "SimpleLitUD.hpp"
#include "LightSource.hpp"

class MovingLightUO : public UniformOperator {

public:

	//Construction
	explicit MovingLightUO(const LightSource &inLight) : light(&inLight) {}

	void operate(UniformData &data, float /*dt*/) override {

		SimpleLitUD *litData = dynamic_cast<SimpleLitUD *>(&data);
		if (!litData) {
			throw std::invalid_argument("MovingLightUO can only operate on SimpleLitUD!");
		}

		litData->lightBlock.position = light->position;
	}

private:

	const LightSource *light;
};