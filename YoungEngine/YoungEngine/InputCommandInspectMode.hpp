#pragma once
#include "GLFWConfig.hpp"

#include <optional>

#include "InputCommand.hpp"
#include "Context.hpp"
#include "Scene.hpp"
#include "Prop.hpp"
#include "SimpleModelUD.hpp"
#include "SimpleLitUD.hpp"

//Switch to Inspect InputMode in Window's InputState
class InputCommandInspectMode : public InputCommand {

public:

	explicit InputCommandInspectMode(GLFWwindow *inWindow) : window(inWindow) {}

	void execute() override {

		if (window == nullptr) return;

		//Reading mouse movement
		if (glfwRawMouseMotionSupported()) {
			glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
		}

		//Enabling cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		//Focusing on a prop
		focusCentralProp();
	}

private:

	GLFWwindow *window;

	void focusCentralProp() {

		//Finds the prop closest to the center of the screen and places it in focus.
		//Also centers the camera onto it.

		if (!Context::active || !Context::active->activeScene) return;

		Context::active->propInFocus = nullptr;

		//Getting references and setting values up
		Scene &scene = *Context::active->activeScene;
		Camera &camera = scene.getCamera();

		glm::vec3 forward = camera.target - camera.position;
		if (glm::length(forward) < 1e-6f) return;
		forward = glm::normalize(forward);

		//Values for iteration
		Prop *closest = nullptr;
		glm::vec3 closestPosition(0.0f);
		float closestAlignment = -2.0f;

		for (const auto &renderable : scene.getRenderables()) {

			//Only props can be in focus
			Prop *prop = dynamic_cast<Prop *>(renderable.get());
			if (!prop) continue;

			//Getting position from appropriate uniform operators
			std::optional<glm::vec3> position = std::nullopt;

			const UniformData &data = prop->getUniformDescription().getData();

			if (const SimpleModelUD *modelData = dynamic_cast<const SimpleModelUD *>(&data))
				position = modelData->position;

			if (const SimpleLitUD *litData = dynamic_cast<const SimpleLitUD *>(&data))
				position = litData->position;

			if (!position) continue;

			//Vector from the prop to the camera, checking its angle towards where camera is looking
			//(larger dot means larger cosine means smaller angle)
			glm::vec3 toProp = *position - camera.position;
			if (glm::length(toProp) < 1e-6f) continue;
			float alignment = glm::dot(forward, glm::normalize(toProp));

			//Updating values
			if (alignment > closestAlignment) {
				closest = prop;
				closestPosition = *position;
				closestAlignment = alignment;
			}
		}

		if (!closest) return;

		Context::active->propInFocus = closest;
		camera.target = closestPosition;
	}
};
