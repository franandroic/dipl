#include "Renderable.hpp"

void Renderable::prepare(const PipelineDescription::RenderingFormats &formats, VkExtent2D extent) {

	prepareTargets(extent);

	if (preparedFormats && *preparedFormats == formats) return;

	resolvePipelines(formats);
	preparedFormats = formats;
}
