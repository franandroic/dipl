#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>
#include <string>

//Contains the window logic, currently wraps GLFWwindow
class Window {

public:

	//Construction
	Window(uint32_t width, uint32_t height, const std::string &title);
	~Window();

	//Copy and Move constructors
	Window(const Window &) = delete;
	Window &operator=(const Window &) = delete;
	Window(Window &&) = delete;
	Window &operator=(Window &&) = delete;

	//Member variables
	GLFWwindow *window = nullptr;
	bool framebufferResized = false;

private:

	static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
};