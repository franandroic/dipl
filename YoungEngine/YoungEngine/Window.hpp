#pragma once
#include "GLFWConfig.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "InputCommand.hpp"
#include "InputCommandReload.hpp"
#include "InputCommandFlyMode.hpp"
#include "InputCommandInspectMode.hpp"

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

	std::vector<std::unique_ptr<InputCommand>> getInputCommands();
	void resumeInput();

private:

	enum class InputMode {
		Fly,
		Inspect
	};

	struct InputState {
		InputMode mode = InputMode::Inspect;
		bool acceptsInput = true;
	};

	InputState inputState;
	std::vector<std::unique_ptr<InputCommand>> pendingInputCommands;

	static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
	static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

	void handleKey(int key, int action);
	std::unique_ptr<InputCommand> createFlyInputCommand(int key, int action);
	std::unique_ptr<InputCommand> createInspectInputCommand(int key, int action);
};