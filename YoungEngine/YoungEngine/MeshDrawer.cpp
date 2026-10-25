#include "MeshDrawer.hpp"

MeshDrawer::MeshDrawer(
	Canvas &inCanvas,
	Pipeline &inPipeline,
	Mesh &inMesh,
	uint32_t inIndexCount,
	const Camera &inCamera
) : canvas(inCanvas),
	pipeline(inPipeline),
	mesh(inMesh),
	indexCount(inIndexCount),
	camera(inCamera) {
}

void MeshDrawer::update(size_t frame) {

	//The object's uniforms are updated by its own operator, while the camera's data
	//goes into the camera uniform buffer of this frame.

	canvas.uniformity.update(frame);
}

void MeshDrawer::record(VkCommandBuffer commandBuffer, size_t frame) {

	//During the recording we bind the pipeline, the vertex and index buffers that specify the data to be drawn,
	//as well as descriptor sets, which we use for things like uniform buffers and samplers.

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.graphicsPipeline);

	VkBuffer vertexBuffers[] = { mesh.VBO.buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(commandBuffer, mesh.IBO.buffer, 0, VK_INDEX_TYPE_UINT32);

	//Each tier of descriptors is bound at its own set number, from the sets of the object that owns the data.
	//The camera's set is one per frame in flight, the texture's is the same in every frame.
	VkDescriptorSet cameraSet = camera.getSet(frame).getHandle();
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, DescriptorTier::Frame, 1, &cameraSet, 0, nullptr);

	VkDescriptorSet textureSet = canvas.texture.getSet().getHandle();
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, DescriptorTier::Material, 1, &textureSet, 0, nullptr);

	//The uniforms of an object only have a set if its data has a buffer block (a model matrix in push constants doesn't)
	if (canvas.uniformity.hasDescriptorSets()) {
		VkDescriptorSet objectSet = canvas.uniformity.getSet(frame).getHandle();
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, DescriptorTier::Object, 1, &objectSet, 0, nullptr);
	}

	canvas.uniformity.recordPushConstants(commandBuffer, pipeline.pipelineLayout);

	//vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0); //For using just the vertex buffer
	vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
}