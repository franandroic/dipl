#include "MeshDrawer.hpp"

MeshDrawer::MeshDrawer(
	Canvas &inCanvas,
	Pipeline &inPipeline,
	Mesh &inMesh,
	uint32_t inIndexCount
) : canvas(inCanvas),
	pipeline(inPipeline),
	mesh(inMesh),
	indexCount(inIndexCount) {
}

void MeshDrawer::update(size_t frame, const Camera &camera) {

	//The object's uniforms are updated by its own operator, while the camera's data
	//goes into the camera uniform buffer of this frame.

	canvas.uniformity.update(frame);

	Camera::UniformBlock cameraBlock = camera.getUniformBlock();
	canvas.UBOs[frame]->updateBuffer(&cameraBlock, sizeof(cameraBlock));
}

void MeshDrawer::record(VkCommandBuffer commandBuffer, size_t frame) {

	//During the recording we bind the pipeline, the vertex and index buffers that specify the data to be drawn,
	//as well as descriptor sets, which we use for things like uniform buffers and samplers.

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.graphicsPipeline);

	VkBuffer vertexBuffers[] = { mesh.VBO.buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(commandBuffer, mesh.IBO.buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, &canvas.description.descriptorSets[frame], 0, nullptr);

	canvas.uniformity.recordPushConstants(commandBuffer, pipeline.pipelineLayout);

	//vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0); //For using just the vertex buffer
	vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
}