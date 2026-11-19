#pragma once
#include "GLFWConfig.hpp"

#include "InputCommand.hpp"

//Switch to Fly InputMode in Window's InputState
class InputCommandFlyMode : public InputCommand {

public:

	explicit InputCommandFlyMode(GLFWwindow *inWindow) : window(inWindow) {}

	void execute() override;

private:

	GLFWwindow *window;
};