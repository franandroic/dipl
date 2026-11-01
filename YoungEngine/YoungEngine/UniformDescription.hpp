#pragma once
#include "GLFWConfig.hpp"

#include <memory>
#include <vector>
#include <stdexcept>
#include <utility>

#include "Device.hpp"
#include "DescriptorSet.hpp"
#include "UniformData.hpp"
#include "UniformOperator.hpp"
#include "UniformBufferObject.hpp"

//Collects everything related to the uniform data of a single object:
//the data (what is passed to the shaders), the operator (how it changes over time),
//the uniform buffers that the often-updated part of the data goes through
//and the descriptor sets pointing at those buffers.
class UniformDescription {

public:

	//Construction
	UniformDescription(
		std::unique_ptr<UniformData> inData,
		std::unique_ptr<UniformOperator> inOp,
		size_t framesInFlight
	);
	~UniformDescription() = default;

	//Copy and Move constructors
	UniformDescription(const UniformDescription &) = delete;
	UniformDescription &operator=(const UniformDescription &) = delete;
	UniformDescription(UniformDescription &&) = delete;
	UniformDescription &operator=(UniformDescription &&) = delete;

	void update(float dt);
	void upload(size_t frame);
	void recordPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) const;

	VkPushConstantRange pushConstantRange() const;

	std::vector<VkDescriptorSetLayoutBinding> bindings() const;

	bool hasDescriptorSets() const { return !descriptorSets.empty(); }
	const DescriptorSet &getSet(size_t frame) const { return *descriptorSets.at(frame); }
	UniformBufferObject *getUBO(size_t frame) const { return UBOs.empty() ? nullptr : UBOs[frame].get(); }
	const UniformData &getData() const { return *data; }

	void setOperator(std::unique_ptr<UniformOperator> inOp);

private:

	static constexpr uint32_t BUFFER_BINDING = 0;
	VkDescriptorSetLayoutBinding makeBinding() const;

	std::unique_ptr<UniformData> data;
	std::unique_ptr<UniformOperator> op;
	std::vector<std::unique_ptr<UniformBufferObject>> UBOs;
	std::vector<std::unique_ptr<DescriptorSet>> descriptorSets;
};