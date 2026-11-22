#pragma once
#include "GLFWConfig.hpp"

#include "InputCommand.hpp"

//Switch to Fly InputMode in Window's InputState
class InputCommandFlyMode : public InputCommand {

public:

	explicit InputCommandFlyMode(GLFWwindow *inWindow) : window(inWindow) {}

	void execute() override {

		if (window == nullptr) return;

		//Disabling cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		//Still reading mouse movement
		if (glfwRawMouseMotionSupported()) {
			glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}

		//Removing any prop from focus
		if (Context::active) Context::active->propInFocus = nullptr;
	}

private:

	GLFWwindow *window;
};