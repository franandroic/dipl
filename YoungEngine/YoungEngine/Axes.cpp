#include "Axes.hpp"

Axes::Axes(
	const PipelineDescription::ShaderPair &inShaders,
	float length,
	const glm::vec3 &position
) : shaders(inShaders),
	modelData(position),
	vertices(makeVertices(length)),
	VBO(vertices, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {

	VBO.createBuffer(vertices);
}

std::vector<Vertex> Axes::makeVertices(float length) {

	//Two vertices per axis (a line list), from the origin to the end of the axis, both in the color of the axis.
	//The colors happen to be the directions themselves. Texture coordinates and normals are left at zero,
	//the axes shaders don't read them.

	const glm::vec3 directions[] = {
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f }
	};

	std::vector<Vertex> result;
	result.reserve(6);

	for (const glm::vec3 &direction : directions) {

		Vertex start{};
		start.pos = glm::vec3(0.0f);
		start.color = direction;

		Vertex end{};
		end.pos = direction * length;
		end.color = direction;

		result.push_back(start);
		result.push_back(end);
	}

	return result;
}

void Axes::resolvePipelines(const PipelineDescription::RenderingFormats &formats) {

	//No material set and no object set, only the model matrix in push constants

	PipelineDescription description;
	description.shaders = shaders;
	description.formats = formats;
	description.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	description.pushConstantRange = modelData.pushConstantRange();

	pipeline = &PipelineCache::active->get(description);
}

void Axes::collect(size_t /*frame*/, std::vector<DrawItem> &out) const {

	//Not indexed: the vertices are drawn in order, two per line

	DrawItem item;
	item.pipeline = pipeline;
	item.pushConstants = modelData.pushConstantBlock();
	item.pushConstantStages = modelData.pushConstantStages();
	item.vertexBuffer = VBO.buffer;
	item.count = static_cast<uint32_t>(vertices.size());

	out.push_back(item);
}
