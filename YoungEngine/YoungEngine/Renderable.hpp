#pragma once
#include "GLFWConfig.hpp"

#include <cstddef>
#include <optional>
#include <vector>

#include "DrawItem.hpp"
#include "PipelineDescription.hpp"

//Abstract class for anything a scene contains that can be drawn.
class Renderable {

public:

	//Construction
	Renderable() = default;
	virtual ~Renderable() = default;

	//Copy and Move constructors
	Renderable(const Renderable &) = delete;
	Renderable &operator=(const Renderable &) = delete;
	Renderable(Renderable &&) = delete;
	Renderable &operator=(Renderable &&) = delete;

	//Game logic, advanced by the time in seconds since the previous update. No Vulkan here.
	//Runs whether (and however often) the renderable gets drawn.
	virtual void update(float /*dt*/) {}

	//Gets the pipelines the renderable is drawn with, for the attachments it's drawn into.
	void prepare(const PipelineDescription::RenderingFormats &formats);

	//Copies the current state into the GPU resources of the frame in flight (its uniform buffers).
	virtual void upload(size_t /*frame*/) {}

	//Appends the draw calls of the renderable for the frame in flight.
	virtual void collect(size_t frame, std::vector<DrawItem> &out) const = 0;

protected:

	//Gets the pipelines from the cache
	virtual void resolvePipelines(const PipelineDescription::RenderingFormats &formats) = 0;

private:

	//The formats the pipelines were resolved for, if they have been
	std::optional<PipelineDescription::RenderingFormats> preparedFormats;
};