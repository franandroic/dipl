#include "Window.hpp"

Window::Window(uint32_t width, uint32_t height, const std::string &title) {

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

Window::~Window() {

	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::framebufferResizeCallback(GLFWwindow *window, int width, int height) {

	auto self = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
	self->framebufferResized = true;
}