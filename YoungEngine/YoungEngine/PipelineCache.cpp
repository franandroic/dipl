#include "PipelineCache.hpp"

#include <cassert>

PipelineCache::PipelineCache() {

	assert(!active && "PipelineCache already exists!");
	active = this;
}

PipelineCache::~PipelineCache() {

	if (active == this) active = nullptr;
}

const Pipeline &PipelineCache::get(const PipelineDescription &description) {

	//Returns the pipeline of the description, creating it if it's the first request for it.

	for (const auto &pipeline : pipelines) {
		if (pipeline->getDescription() == description) return *pipeline;
	}

	//Nothing is added if the creation throws
	pipelines.push_back(std::make_unique<Pipeline>(description));

	return *pipelines.back();
}
