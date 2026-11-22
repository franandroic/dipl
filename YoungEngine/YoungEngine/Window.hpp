#pragma once
#include "GLFWConfig.hpp"
#include "GLMConfig.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "Context.hpp"
#include "InputCommand.hpp"
#include "InputCommandReload.hpp"
#include "InputCommandFlyMode.hpp"
#include "InputCommandInspectMode.hpp"
#include "InputCommandMoveCamera.hpp"
#include "InputCommandTurnCamera.hpp"
#include "InputCommandRotateFocused.hpp"

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
		glm::dvec2 lastCursorPos{ 0.0 };
		bool cursorCaptured = false;
	};

	InputState inputState;
	std::vector<std::unique_ptr<InputCommand>> pendingInputCommands;

	static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
	static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void cursorPosCallback(GLFWwindow *window, double x, double y);

	void handleKey(int key, int action);
	void handleCursor(double x, double y);

	std::unique_ptr<InputCommand> createFlyKeyInputCommand(int key, int action);
	std::unique_ptr<InputCommand> createInspectKeyInputCommand(int key, int action);
	std::unique_ptr<InputCommand> createFlyCursorInputCommand(double x, double y);
	std::unique_ptr<InputCommand> createInspectCursorInputCommand(double x, double y);
};