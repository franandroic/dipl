#pragma once
#include "GLFWConfig.hpp"

#include "InputCommand.hpp"

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
	}

private:

	GLFWwindow *window;
};
