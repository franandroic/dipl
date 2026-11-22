#pragma once
#include "GLMConfig.hpp"

#include "InputCommand.hpp"
#include "Context.hpp"
#include "Scene.hpp"
#include "Prop.hpp"
#include "MouseDragUO.hpp"

class InputCommandRotateFocused : public InputCommand {

public:

	explicit InputCommandRotateFocused(glm::vec3 inMotion) : motion(inMotion) {}

	void execute() override {

		if (!Context::active || !Context::active->activeScene || !Context::active->propInFocus) return;

		//Getting the appropriate uniform operator from the prop in focus
		MouseDragUO *dragOperator = dynamic_cast<MouseDragUO *>(Context::active->propInFocus->getUniformOperator());
		if (!dragOperator) return;

		Camera &camera = Context::active->activeScene->getCamera();

		//world-space camera calculations
		glm::vec3 forward = camera.target - camera.position;
		glm::vec3 right = glm::cross(forward, camera.up);
		glm::vec3 localUp = glm::vec3(0.0f);

		if (glm::length(forward) < 1e-6f || glm::length(right) < 1e-6f) return;

		forward = glm::normalize(forward);
		right = glm::normalize(right);
		localUp = glm::cross(right, forward);

		//projection of the motion to camera's plane
		glm::vec3 worldMotion = right * glm::dot(motion, Context::CAMERA_RIGHT)
							  + localUp * glm::dot(motion, Context::CAMERA_UP);

		float length = glm::length(worldMotion);
		if (length < 1e-6f) return;

		//Rotation axis, orthogonal to the mouse motion vector
		glm::vec3 axis = glm::cross(worldMotion, forward);

		dragOperator->drag(length * rotationSpeed, axis);
	}

private:

	glm::vec3 motion;
	float rotationSpeed = 0.01f;
};