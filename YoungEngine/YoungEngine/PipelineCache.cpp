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

void PipelineCache::rebuild(const std::vector<std::string> &changedSources) {

	//Rebuilds every pipeline that runs one of the shader sources, waiting for the device to go idle first.
	//Has to be called between frames. A pipeline that fails to rebuild keeps its old one and the error is printed.

	auto changed = [&changedSources](const std::string &source) {
		return std::find(changedSources.begin(), changedSources.end(), source) != changedSources.end();
	};

	bool idle = false;

	for (const auto &pipeline : pipelines) {

		const PipelineDescription::ShaderSource &shaders = pipeline->getDescription().shaders;
		if (!changed(shaders.vertex) && !changed(shaders.fragment)) continue;

		if (!idle) {
			vkDeviceWaitIdle(Device::active->logical);
			idle = true;
		}

		try {
			pipeline->rebuild();
		} catch (const std::exception &e) {
			std::cerr << "Failed to rebuild the pipeline of " << shaders.vertex << " and " << shaders.fragment << ": " << e.what() << std::endl;
		}
	}
}