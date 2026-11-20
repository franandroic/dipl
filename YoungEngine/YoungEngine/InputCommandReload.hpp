#pragma once

#include "InputCommand.hpp"

#include "ShaderCompiler.hpp"
#include "PipelineCache.hpp"

//Triggers recompiling and reloading shaders
class InputCommandReload : public InputCommand {

public:

	void execute() override {

		if (!ShaderCompiler::active || !PipelineCache::active) return;
		PipelineCache::active->rebuild(ShaderCompiler::active->recompile());
	}

	bool blocksInput() const override { return true; }
};