#include "GameObject.hpp"

GameObject::GameObject(
	PipelineCache &pipelineCache,
	const Camera &inCamera,
	const PipelineDescription::RenderingFormats &formats,
	std::unique_ptr<Mesh> inMesh,
	std::unique_ptr<Material> inMaterial
) : camera(inCamera),
	mesh(std::move(inMesh)),
	material(std::move(inMaterial)) {

	if (!mesh || !material) {
		throw std::invalid_argument("A game object requires a mesh and a material!");
	}

	//Describing the pipeline that draws this mesh with this material and getting it from the cache,
	//which creates it if no other object has asked for an equal one before.

	PipelineDescription description;
	description.shaders = material->getShaders();
	description.formats = formats;
	description.materialBindings = material->bindings();
	description.objectBindings = material->getUniformity().bindings();
	description.pushConstantRange = material->getUniformity().pushConstantRange();

	pipeline = &pipelineCache.get(description);
}

void GameObject::update(size_t frame) {

	//The material's uniforms are updated by its own operator, while the camera's data
	//goes into the camera uniform buffer of this frame (the Renderer does that before this is called).

	material->update(frame);
}

void GameObject::record(VkCommandBuffer commandBuffer, size_t frame) {

	//During the recording we bind the pipeline, the vertex and index buffers that specify the data to be drawn,
	//as well as descriptor sets, which we use for things like uniform buffers and samplers.

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->graphicsPipeline);

	VkBuffer vertexBuffers[] = { mesh->VBO.buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(commandBuffer, mesh->IBO.buffer, 0, VK_INDEX_TYPE_UINT32);

	//The camera's set is one per frame in flight
	VkDescriptorSet cameraSet = camera.getSet(frame).getHandle();
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipelineLayout, PipelineDescription::DescriptorTier::Frame, 1, &cameraSet, 0, nullptr);

	//The material binds the rest of its sets and its push constants
	material->apply(commandBuffer, pipeline->pipelineLayout, frame);

	//vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0); //For using just the vertex buffer
	vkCmdDrawIndexed(commandBuffer, mesh->IBO.indexCount, 1, 0, 0, 0);
}
