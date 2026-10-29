#include "GameObject.hpp"

GameObject::GameObject(
	Device &inDevice,
	const Mesh &inMesh,
	const Material &inMaterial,
	std::unique_ptr<UniformData> inUniformData,
	std::unique_ptr<UniformOperator> inUniformOperator
) : mesh(&inMesh),
	material(&inMaterial),
	uniformity(inDevice, std::move(inUniformData), std::move(inUniformOperator), MAX_FRAMES_IN_FLIGHT) {
}

void GameObject::update(float dt) {

	uniformity.update(dt);
}

void GameObject::upload(size_t frame) {

	uniformity.upload(frame);
}

void GameObject::resolvePipelines(PipelineCache &pipelineCache, const PipelineDescription::RenderingFormats &formats) {

	//The pipeline depends on the material (shaders, texture set) and on the kind of uniform data of the object
	//(object set, push constants). Objects that are alike in both get the same pipeline from the cache.

	PipelineDescription description;
	description.shaders = material->getShaders();
	description.formats = formats;
	description.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	description.materialBindings = material->bindings();
	description.objectBindings = uniformity.bindings();
	description.pushConstantRange = uniformity.pushConstantRange();

	pipeline = &pipelineCache.get(description);
}

void GameObject::collect(size_t frame, std::vector<DrawItem> &out) const {

	const UniformData &data = uniformity.getData();

	DrawItem item;
	item.pipeline = pipeline;
	item.materialSet = material->getSet();
	item.objectSet = uniformity.hasDescriptorSets() ? uniformity.getSet(frame).getHandle() : VK_NULL_HANDLE;
	item.pushConstants = data.pushConstantBlock();
	item.pushConstantStages = data.pushConstantStages();
	item.vertexBuffer = mesh->VBO.buffer;
	item.indexBuffer = mesh->IBO.buffer;
	item.count = mesh->IBO.indexCount;

	out.push_back(item);
}
