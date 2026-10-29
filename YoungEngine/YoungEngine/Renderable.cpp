#include "Renderable.hpp"

void Renderable::prepare(PipelineCache &pipelineCache, const PipelineDescription::RenderingFormats &formats) {

	if (preparedFormats && *preparedFormats == formats) return;

	resolvePipelines(pipelineCache, formats);
	preparedFormats = formats;
}
