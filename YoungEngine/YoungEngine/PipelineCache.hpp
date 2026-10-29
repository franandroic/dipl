#pragma once
#include "GLFWConfig.hpp"

#include <cstddef>
#include <memory>
#include <vector>

#include "Device.hpp"
#include "Pipeline.hpp"
#include "PipelineDescription.hpp"

//Hands out pipelines for descriptions.
class PipelineCache {

public:

	//Construction
	explicit PipelineCache(Device &inDevice) : device(inDevice) {}
	~PipelineCache() = default;

	//Copy and Move constructors
	PipelineCache(const PipelineCache &) = delete;
	PipelineCache &operator=(const PipelineCache &) = delete;
	PipelineCache(PipelineCache &&) = delete;
	PipelineCache &operator=(PipelineCache &&) = delete;

	//Returns the pipeline of the description, creating it if it's the first request for it.
	const Pipeline &get(const PipelineDescription &description);

	//Number of pipelines created so far
	size_t size() const { return pipelines.size(); }

private:

	Device &device;

	std::vector<std::unique_ptr<Pipeline>> pipelines;
};