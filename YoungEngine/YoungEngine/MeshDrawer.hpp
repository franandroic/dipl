#pragma once
#include "GLFWConfig.hpp"

#include "Drawer.hpp"
#include "Canvas.hpp"
#include "Pipeline.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"

//Draws an object using a graphics pipeline.
class MeshDrawer : public Drawer {

public:

	MeshDrawer(
		Canvas &canvas,
		Pipeline &pipeline,
		Mesh &mesh,
		uint32_t indexCount
	);

	void update(size_t frame, const Camera &camera) override;
	void record(VkCommandBuffer commandBuffer, size_t frame) override;

private:

	Canvas &canvas;
	Pipeline &pipeline;
	Mesh &mesh;
	uint32_t indexCount;
};