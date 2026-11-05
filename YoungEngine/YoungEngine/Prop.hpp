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
#include "UniformDescription.hpp"

//An object in the scene: a mesh (what it's shaped like) drawn with a material (what it looks like).
class Prop : public Renderable {

public:

	//Construction
	Prop(
		const Mesh &inMesh,
		Material &inMaterial,
		std::unique_ptr<UniformData> inUniformData,
		std::unique_ptr<UniformOperator> inUniformOperator = nullptr
	);
	~Prop() override = default;

	const Mesh &getMesh() const { return *mesh; }
	const Material &getMaterial() const { return *material; }
	const UniformDescription &getUniformDescription() const { return uniforms; }

	void update(float dt) override;
	void upload(size_t frame) override;
	void collect(size_t frame, std::vector<DrawItem> &out) const override;

protected:

	void prepareTargets(VkExtent2D extent) override;
	void resolvePipelines(const PipelineDescription::RenderingFormats &formats) override;

private:

	const Mesh *mesh;
	Material *material;
	UniformDescription uniforms;

	//One for each pass of the material, owned by the pipeline cache
	std::vector<const Pipeline *> pipelines;
};