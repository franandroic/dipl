#include "Uniformity.hpp"

#include <stdexcept>
#include <utility>

Uniformity::Uniformity(
	Device &inDevice,
	std::unique_ptr<UniformData> inData,
	std::unique_ptr<UniformOperator> inOp,
	size_t framesInFlight
) : device(inDevice),
	data(std::move(inData)),
	op(std::move(inOp)) {

	if (!data) {
		throw std::invalid_argument("Uniformity requires uniform data!");
	}

	UniformData::Block block = data->bufferBlock();
	if (block.empty()) return;

	UBOs.reserve(framesInFlight);

	for (size_t i = 0; i < framesInFlight; i++) {
		UBOs.push_back(std::make_unique<UniformBufferObject>(
			inDevice,
			block.size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		));
		UBOs[i]->createBuffer();
	}
}

void Uniformity::setOperator(std::unique_ptr<UniformOperator> inOp) {

	op = std::move(inOp);
}

void Uniformity::update(size_t frame) {

	if (op) op->operate(*data);

	if (UBOs.empty()) return;

	UniformData::Block block = data->bufferBlock();
	UBOs[frame]->updateBuffer(block.data, block.size);
}

void Uniformity::recordPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) const {

	UniformData::Block block = data->pushConstantBlock();
	if (block.empty()) return;

	vkCmdPushConstants(commandBuffer, pipelineLayout, data->pushConstantStages(), 0, static_cast<uint32_t>(block.size), block.data);
}

VkPushConstantRange Uniformity::pushConstantRange() const {

	return data->pushConstantRange();
}

std::vector<VkDescriptorSetLayoutBinding> Uniformity::bindings() const {

	if (UBOs.empty()) return {};

	return {
		Description::makeBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, data->bufferStages())
	};
}

void Uniformity::createDescriptorSets(const Description &description) {

	//The buffers stay the same for the whole lifetime of the object, so the sets are written only once.

	descriptorSets.clear();
	descriptorSets.reserve(UBOs.size());

	for (size_t i = 0; i < UBOs.size(); i++) {
		descriptorSets.push_back(std::make_unique<DescriptorSet>(device, description));
		descriptorSets[i]->writeUniformBuffer(0, *UBOs[i]);
	}
}

UniformBufferObject *Uniformity::getUBO(size_t frame) const {

	return UBOs.empty() ? nullptr : UBOs[frame].get();
}