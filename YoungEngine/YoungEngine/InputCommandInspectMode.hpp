#pragma once
#include "GLFWConfig.hpp"

#include "InputCommand.hpp"

//Switch to Inspect InputMode in Window's InputState
class InputCommandInspectMode : public InputCommand {

public:

	explicit InputCommandInspectMode(GLFWwindow *inWindow) : window(inWindow) {}

	void execute() override;

private:

	GLFWwindow *window;
};
