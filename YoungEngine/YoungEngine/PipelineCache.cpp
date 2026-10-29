#include "PipelineCache.hpp"

const Pipeline &PipelineCache::get(const PipelineDescription &description) {

	for (const auto &pipeline : pipelines) {
		if (pipeline->getDescription() == description) return *pipeline;
	}

	//Nothing is added if the creation throws
	pipelines.push_back(std::make_unique<Pipeline>(device, description));

	return *pipelines.back();
}
