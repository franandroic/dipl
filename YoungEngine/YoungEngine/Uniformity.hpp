#pragma once
#include "GLFWConfig.hpp"

#include <memory>
#include <vector>

#include "Device.hpp"
#include "UniformData.hpp"
#include "UniformOperator.hpp"
#include "UniformBufferObject.hpp"

//Collects everything related to the uniform data of a single object:
//the data (what is passed to the shaders), the operator (how it changes over time)
//and the uniform buffers that the often-updated part of the data goes through.
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

	//Uniform buffer of the given frame, or nullptr if the data has no buffer block.
	UniformBufferObject *getUBO(size_t frame) const;

	UniformData &getData() { return *data; }
	void setOperator(std::unique_ptr<UniformOperator> inOp);

private:

	std::unique_ptr<UniformData> data;
	std::unique_ptr<UniformOperator> op;
	std::vector<std::unique_ptr<UniformBufferObject>> UBOs;
};