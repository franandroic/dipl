#include "Renderable.hpp"

void Renderable::prepare(const PipelineDescription::RenderingFormats &formats) {

	if (preparedFormats && *preparedFormats == formats) return;

	resolvePipelines(formats);
	preparedFormats = formats;
}
