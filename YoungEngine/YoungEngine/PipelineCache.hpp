#pragma once
#include "GLFWConfig.hpp"

#include <cstddef>
#include <memory>
#include <vector>
#include <assert.h>

#include "Device.hpp"
#include "Pipeline.hpp"
#include "PipelineDescription.hpp"

//Holds a vector of pipelines with unique layouts.
//Creates a new pipeline for a new unique layout description and adds it to the vector.
//Hands out pipeline references on request.
class PipelineCache {

public:

	//Construction
	PipelineCache();
	~PipelineCache();

	//Copy and Move constructors
	PipelineCache(const PipelineCache &) = delete;
	PipelineCache &operator=(const PipelineCache &) = delete;
	PipelineCache(PipelineCache &&) = delete;
	PipelineCache &operator=(PipelineCache &&) = delete;

	//Global access
	static inline PipelineCache *active = nullptr;

	const Pipeline &get(const PipelineDescription &description);
	size_t size() const { return pipelines.size(); }

private:

	std::vector<std::unique_ptr<Pipeline>> pipelines;
};