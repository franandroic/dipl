#include "InputCommandInspectMode.hpp"

void InputCommandInspectMode::execute() {

	if (window == nullptr) return;

	//Reading mouse movement
	if (glfwRawMouseMotionSupported()) {
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
	}

	//Enabling cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
