#pragma once
#include "GLFWConfig.hpp"

#include <memory>
#include <stdexcept>

#include "Device.hpp"
#include "Renderable.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "Pipeline.hpp"
#include "PipelineCache.hpp"
#include "PipelineDescription.hpp"
#include "UniformData.hpp"
#include "UniformOperator.hpp"
#include "Uniformity.hpp"

//An object in the scene: a mesh (what it's shaped like) drawn with a material (what it looks like).
class GameObject : public Renderable {

public:

	//Construction
	GameObject(
		Device &inDevice,
		const Mesh &inMesh,
		const Material &inMaterial,
		std::unique_ptr<UniformData> inUniformData,
		std::unique_ptr<UniformOperator> inUniformOperator = nullptr
	);
	~GameObject() override = default;

	const Mesh &getMesh() const { return *mesh; }
	const Material &getMaterial() const { return *material; }
	Uniformity &getUniformity() { return uniformity; }

	void update(float dt) override;
	void upload(size_t frame) override;
	void collect(size_t frame, std::vector<DrawItem> &out) const override;

protected:

	void resolvePipelines(PipelineCache &pipelineCache, const PipelineDescription::RenderingFormats &formats) override;

private:

	const Mesh *mesh;
	const Material *material;
	Uniformity uniformity;

	//Owned by the pipeline cache
	const Pipeline *pipeline = nullptr;
};