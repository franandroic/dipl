#include "InputCommandFlyMode.hpp"

void InputCommandFlyMode::execute() {

	if (window == nullptr) return;

	//Disabling cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Still reading mouse movement
	if (glfwRawMouseMotionSupported()) {
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
}