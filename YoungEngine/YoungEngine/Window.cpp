#include "Window.hpp"

Window::Window(uint32_t width, uint32_t height, const std::string &title) {

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);

	glfwGetCursorPos(window, &inputState.lastCursorPos.x, &inputState.lastCursorPos.y);
}

Window::~Window() {

	glfwDestroyWindow(window);
	glfwTerminate();
}

std::vector<std::unique_ptr<InputCommand>> Window::getInputCommands() {

	std::vector<std::unique_ptr<InputCommand>> inputCommands = std::move(pendingInputCommands);
	pendingInputCommands.clear();

	return inputCommands;
}

void Window::resumeInput() {
	
	//If any input comes while it's not being accepted, we flush it by polling here

	if (inputState.acceptsInput) return;

	glfwPollEvents();
	inputState.acceptsInput = true;
}

void Window::framebufferResizeCallback(GLFWwindow *window, int width, int height) {

	auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
	self->framebufferResized = true;
}

void Window::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {

	auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
	self->handleKey(key, action);
}

void Window::cursorPosCallback(GLFWwindow *window, double x, double y) {

	auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
	self->handleCursor(x, y);
}

void Window::handleKey(int key, int action) {

	//Reacts to key callbacks by going through possible input signals and creating appropriate
	//commands only for the InputState it's currently in. Returns early if the state stops
	//accepting input.

	if (!inputState.acceptsInput) return;

	std::unique_ptr<InputCommand> inputCommand;
	switch (inputState.mode) {

	case InputMode::Fly:
		inputCommand = createFlyInputCommand(key, action);
		break;

	case InputMode::Inspect:
		inputCommand = createInspectInputCommand(key, action);
		break;
	}

	if (!inputCommand) return;
	if (inputCommand->blocksInput()) inputState.acceptsInput = false;

	pendingInputCommands.push_back(std::move(inputCommand));
}

void Window::handleCursor(double x, double y) {

	//Recording the cursor's position
	glm::dvec2 currCursorPos(x, y);
	glm::dvec2 offset = currCursorPos - inputState.lastCursorPos;
	inputState.lastCursorPos = currCursorPos;
	
	//If we're changing cursor's state we don't want to create a command
	bool captured = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
	if (captured != inputState.cursorCaptured) {
		inputState.cursorCaptured = captured;
		return;
	}

	if (!inputState.acceptsInput) return;

	std::unique_ptr<InputCommand> inputCommand;
	switch (inputState.mode) {

	case InputMode::Fly:
		inputCommand = createFlyCursorCommand(offset.x, offset.y);
		break;

	case InputMode::Inspect:
		break;
	}

	if (!inputCommand) return;
	if (inputCommand->blocksInput()) inputState.acceptsInput = false;

	pendingInputCommands.push_back(std::move(inputCommand));
}

std::unique_ptr<InputCommand> Window::createFlyInputCommand(int key, int action) {

	switch (key) {

	case GLFW_KEY_TAB:
		if (action == GLFW_PRESS) {
			inputState.mode = InputMode::Inspect;
			return std::make_unique<InputCommandInspectMode>(window);
		}
		break;

	case GLFW_KEY_R:
		if (action == GLFW_PRESS) {
			return std::make_unique<InputCommandReload>();
		}
		break;

	case GLFW_KEY_W:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			return std::make_unique<InputCommandMoveCamera>(Context::CAMERA_FORWARD);
		}
		break;

	case GLFW_KEY_A:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			return std::make_unique<InputCommandMoveCamera>(-Context::CAMERA_RIGHT);
		}
		break;

	case GLFW_KEY_S:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			return std::make_unique<InputCommandMoveCamera>(-Context::CAMERA_FORWARD);
		}
		break;

	case GLFW_KEY_D:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			return std::make_unique<InputCommandMoveCamera>(Context::CAMERA_RIGHT);
		}
		break;

	case GLFW_KEY_Q:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			return std::make_unique<InputCommandMoveCamera>(Context::CAMERA_UP);
		}
		break;

	case GLFW_KEY_E:
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			return std::make_unique<InputCommandMoveCamera>(-Context::CAMERA_UP);
		}
		break;

	default:
		break;
	}

	return nullptr;
}

std::unique_ptr<InputCommand> Window::createInspectInputCommand(int key, int action) {

	switch (key) {

	case GLFW_KEY_TAB:
		if (action == GLFW_PRESS) {
			inputState.mode = InputMode::Fly;
			return std::make_unique<InputCommandFlyMode>(window);
		}
		break;

	case GLFW_KEY_R:
		if (action == GLFW_PRESS) {
			return std::make_unique<InputCommandReload>();
		}
		break;

	default:
		break;
	}

	return nullptr;
}

std::unique_ptr<InputCommand> Window::createFlyCursorCommand(double x, double y) {

	return std::make_unique<InputCommandTurnCamera>(
		Context::CAMERA_RIGHT * static_cast<float>(x) - Context::CAMERA_UP * static_cast<float>(y)
	);
}