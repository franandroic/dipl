#include "Prop.hpp"

Prop::Prop(
	const Mesh &inMesh,
	Material &inMaterial,
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

void Prop::prepareTargets(VkExtent2D extent) {

	material->prepareTargets(extent);
}

void Prop::resolvePipelines(const PipelineDescription::RenderingFormats &formats) {

	//Each pass gets its own pipeline, from the pass (shaders, state, the bindings of its textures, where it renders)
	//and, for a pass that draws the mesh, from the kind of uniform data of the object (object set, push constants).
	//A fullscreen pass doesn't draw the object, so it gets none of the object's data.
	//Passes and objects that are alike in all of that get the same pipeline from the cache.

	pipelines.clear();
	pipelines.reserve(material->passCount());

	for (size_t i = 0; i < material->passCount(); i++) {

		const MaterialPass &pass = material->getPass(i);
		bool fullscreen = pass.geometry == MaterialPass::Geometry::Fullscreen;

		PipelineDescription description;
		description.shaders = pass.shaders;
		description.formats = material->renderingFormats(i, formats);
		description.raster = pass.raster;
		description.vertexInput = fullscreen ? PipelineDescription::VertexInput::None : PipelineDescription::VertexInput::Mesh;
		description.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		description.materialBindings = material->bindings(i);

		if (!fullscreen) {
			description.objectBindings = uniforms.bindings();
			description.pushConstantRange = uniforms.pushConstantRange();
		}

		pipelines.push_back(&PipelineCache::active->get(description));
	}
}

void Prop::collect(size_t frame, std::vector<DrawItem> &out) const {

	//One item for each pass. A fullscreen pass is collected by every prop that uses the material,
	//and the renderer keeps only one of them.

	const UniformData &data = uniforms.getData();

	for (size_t i = 0; i < material->passCount(); i++) {

		DrawItem item;
		item.pipeline = pipelines[i];
		item.material = material;
		item.pass = static_cast<uint32_t>(i);
		item.materialSet = material->getSet(i);

		if (material->getPass(i).geometry == MaterialPass::Geometry::Fullscreen) {
			item.count = 3;
		} else {
			item.objectSet = uniforms.hasDescriptorSets() ? uniforms.getSet(frame).getHandle() : VK_NULL_HANDLE;
			item.pushConstants = data.pushConstantBlock();
			item.pushConstantStages = data.pushConstantStages();
			item.vertexBuffer = mesh->VBO.buffer;
			item.indexBuffer = mesh->IBO.buffer;
			item.count = mesh->IBO.indexCount;
		}

		out.push_back(item);
	}
}
