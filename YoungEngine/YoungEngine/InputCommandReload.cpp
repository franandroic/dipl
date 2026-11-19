#include "InputCommandReload.hpp"

void InputCommandReload::execute() {

	if (!ShaderCompiler::active || !PipelineCache::active) return;
	PipelineCache::active->rebuild(ShaderCompiler::active->recompile());
}