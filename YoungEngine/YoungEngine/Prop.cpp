#include "Prop.hpp"

Prop::Prop(
	const Mesh &inMesh,
	const Material &inMaterial,
	std::unique_ptr<UniformData> inUniformData,
	std::unique_ptr<UniformOperator> inUniformOperator
) : mesh(&inMesh),
	material(&inMaterial),
	uniforms(std::move(inUniformData), std::move(inUniformOperator), MAX_FRAMES_IN_FLIGHT) {
}

void Prop::update(float dt) {

	uniforms.update(dt);
}

void Prop::upload(size_t frame) {

	uniforms.upload(frame);
}

void Prop::resolvePipelines(const PipelineDescription::RenderingFormats &formats) {

	//The pipeline depends on the material (shaders, texture set) and on the kind of uniform data of the object
	//(object set, push constants). Objects that are alike in both get the same pipeline from the cache.

	PipelineDescription description;
	description.shaders = material->getShaders();
	description.formats = formats;
	description.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	description.materialBindings = material->bindings();
	description.objectBindings = uniforms.bindings();
	description.pushConstantRange = uniforms.pushConstantRange();

	pipeline = &PipelineCache::active->get(description);
}

void Prop::collect(size_t frame, std::vector<DrawItem> &out) const {

	const UniformData &data = uniforms.getData();

	DrawItem item;
	item.pipeline = pipeline;
	item.materialSet = material->getSet();
	item.objectSet = uniforms.hasDescriptorSets() ? uniforms.getSet(frame).getHandle() : VK_NULL_HANDLE;
	item.pushConstants = data.pushConstantBlock();
	item.pushConstantStages = data.pushConstantStages();
	item.vertexBuffer = mesh->VBO.buffer;
	item.indexBuffer = mesh->IBO.buffer;
	item.count = mesh->IBO.indexCount;

	out.push_back(item);
}
