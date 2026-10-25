#pragma once
#include "GLFWConfig.hpp"

#include <memory>
#include <vector>

#include "Device.hpp"
#include "Description.hpp"
#include "DescriptorSet.hpp"
#include "UniformData.hpp"
#include "UniformOperator.hpp"
#include "UniformBufferObject.hpp"

//Collects everything related to the uniform data of a single object:
//the data (what is passed to the shaders), the operator (how it changes over time),
//the uniform buffers that the often-updated part of the data goes through
//and the descriptor sets pointing at those buffers.
class Uniformity {

public:

	//Construction
	Uniformity(
		Device &inDevice,
		std::unique_ptr<UniformData> inData,
		std::unique_ptr<UniformOperator> inOp,
		size_t framesInFlight
	);
	~Uniformity() = default;

	//Copy and Move constructors
	Uniformity(const Uniformity &) = delete;
	Uniformity &operator=(const Uniformity &) = delete;
	Uniformity(Uniformity &&) = delete;
	Uniformity &operator=(Uniformity &&) = delete;

	//Runs the operator on the data and uploads the buffer block (if there is one) to the uniform buffer of the frame.
	void update(size_t frame);

	//Records the push constants (if there are any) into a command buffer that's being recorded.
	//The pipeline has to be bound, and its layout has to declare the range given by pushConstantRange().
	void recordPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) const;

	//The range to declare in the pipeline layout. A range of size 0 means there are no push constants.
	VkPushConstantRange pushConstantRange() const;

	//The shape of the descriptor set of the object (DescriptorTier::Object), to create a pipeline with.
	//Empty if the data has no buffer block.
	std::vector<VkDescriptorSetLayoutBinding> bindings() const;

	//Allocates a descriptor set for each frame in flight, pointing at the uniform buffer of the frame.
	//Does nothing if the data has no buffer block. The description has to be made from bindings().
	void createDescriptorSets(const Description &description);

	bool hasDescriptorSets() const { return !descriptorSets.empty(); }

	//Throws std::out_of_range if there's no descriptor set (see hasDescriptorSets)
	const DescriptorSet &getSet(size_t frame) const { return *descriptorSets.at(frame); }

	//Uniform buffer of the given frame, or nullptr if the data has no buffer block.
	UniformBufferObject *getUBO(size_t frame) const;

	UniformData &getData() { return *data; }
	void setOperator(std::unique_ptr<UniformOperator> inOp);

private:

	Device &device;

	std::unique_ptr<UniformData> data;
	std::unique_ptr<UniformOperator> op;
	std::vector<std::unique_ptr<UniformBufferObject>> UBOs;
	std::vector<std::unique_ptr<DescriptorSet>> descriptorSets;
};