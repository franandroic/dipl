#pragma once
#include "GLMConfig.hpp"

#include "InputCommand.hpp"
#include "Context.hpp"
#include "Scene.hpp"

class InputCommandMoveCamera : public InputCommand {

public:

	explicit InputCommandMoveCamera(glm::vec3 inDirection) : direction(inDirection) {}

	void execute() override {

		if (!Context::active || !Context::active->activeScene) return;

		Context::active->activeScene->getCamera().move(direction);
	}

private:

	glm::vec3 direction;
};