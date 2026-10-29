#pragma once
#include "GLFWConfig.hpp"
#include "GLMConfig.hpp"

#include <vector>

#include "Device.hpp"
#include "Renderable.hpp"
#include "Pipeline.hpp"
#include "PipelineCache.hpp"
#include "PipelineDescription.hpp"
#include "Vertex.hpp"
#include "VertexBufferObject.hpp"
#include "SimpleModelUD.hpp"

class Axes : public Renderable {

public:

	//Construction
	Axes(
		Device &inDevice,
		const PipelineDescription::ShaderPair &inShaders,
		float length = 1.0f,
		const glm::vec3 &position = glm::vec3(0.0f)
	);
	~Axes() override = default;

	void collect(size_t frame, std::vector<DrawItem> &out) const override;

protected:

	void resolvePipelines(PipelineCache &pipelineCache, const PipelineDescription::RenderingFormats &formats) override;

private:

	static std::vector<Vertex> makeVertices(float length);

	PipelineDescription::ShaderPair shaders;

	//Only the model matrix, passed as a push constant. The axes don't move, so there's no operator.
	SimpleModelUD modelData;

	//Declared before the buffer, which is sized from them
	std::vector<Vertex> vertices;
	VertexBufferObject VBO;

	//Owned by the pipeline cache
	const Pipeline *pipeline = nullptr;
};