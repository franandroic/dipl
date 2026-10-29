#pragma once
#include "GLFWConfig.hpp"

#include <memory>
#include <stdexcept>

#include "Drawer.hpp"
#include "Camera.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Pipeline.hpp"
#include "PipelineCache.hpp"
#include "PipelineDescription.hpp"

//An object in the scene: a mesh (what it's shaped like) drawn with a material (what it looks like).
class GameObject : public Drawer {

public:

	//Construction
	GameObject(
		PipelineCache &pipelineCache,
		const Camera &inCamera,
		const PipelineDescription::RenderingFormats &formats,
		std::unique_ptr<Mesh> inMesh,
		std::unique_ptr<Material> inMaterial
	);
	~GameObject() override = default;

	//Copy and Move constructors
	GameObject(const GameObject &) = delete;
	GameObject &operator=(const GameObject &) = delete;
	GameObject(GameObject &&) = delete;
	GameObject &operator=(GameObject &&) = delete;

	Mesh &getMesh() { return *mesh; }
	Material &getMaterial() { return *material; }
	const Pipeline &getPipeline() const { return *pipeline; }

	void update(size_t frame) override;
	void record(VkCommandBuffer commandBuffer, size_t frame) override;

private:

	const Camera &camera;

	std::unique_ptr<Mesh> mesh;
	std::unique_ptr<Material> material;

	//Owned by the pipeline cache
	const Pipeline *pipeline = nullptr;
};