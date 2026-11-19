#pragma once

//Command interface for input commands. Window creates it.
class InputCommand {

public:

	//Construction
	InputCommand() = default;
	virtual ~InputCommand() = default;

	//Copy and Move constructors
	InputCommand(const InputCommand &) = delete;
	InputCommand &operator=(const InputCommand &) = delete;
	InputCommand(InputCommand &&) = delete;
	InputCommand &operator=(InputCommand &&) = delete;

	virtual void execute() = 0;
	virtual bool blocksInput() const { return false; }
};