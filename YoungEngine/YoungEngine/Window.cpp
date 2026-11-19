#include "Window.hpp"

Window::Window(uint32_t width, uint32_t height, const std::string &title) {

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
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