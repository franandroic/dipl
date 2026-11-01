#include "UniformDescription.hpp"

UniformDescription::UniformDescription(
	std::unique_ptr<UniformData> inData,
	std::unique_ptr<UniformOperator> inOp,
	size_t framesInFlight
) : data(std::move(inData)),
	op(std::move(inOp)) {

	if (!data) {
		throw std::invalid_argument("UniformDescription requires uniform data!");
	}

	UniformData::Block block = data->bufferBlock();
	if (block.empty()) return;

	UBOs.reserve(framesInFlight);
	descriptorSets.reserve(framesInFlight);

	for (size_t i = 0; i < framesInFlight; i++) {
		UBOs.push_back(std::make_unique<UniformBufferObject>(
			block.size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		));
		UBOs[i]->createBuffer();

		descriptorSets.push_back(std::make_unique<DescriptorSet>(bindings()));
		descriptorSets[i]->writeUniformBuffer(BUFFER_BINDING, *UBOs[i]);
	}
}

void UniformDescription::setOperator(std::unique_ptr<UniformOperator> inOp) {

	op = std::move(inOp);
}

void UniformDescription::update(float dt) {

	//Runs the operator (if there is one) on the data.

	if (op) op->operate(*data, dt);
}

void UniformDescription::upload(size_t frame) {

	//Copies the buffer block (if there is one) into the uniform buffer of the frame.

	if (UBOs.empty()) return;

	UniformData::Block block = data->bufferBlock();
	UBOs[frame]->updateBuffer(block.data, block.size);
}

void UniformDescription::recordPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) const {

	//Records the push constants (if there are any) into a command buffer that's being recorded.

	UniformData::Block block = data->pushConstantBlock();
	if (block.empty()) return;

	vkCmdPushConstants(commandBuffer, pipelineLayout, data->pushConstantStages(), 0, static_cast<uint32_t>(block.size), block.data);
}

VkPushConstantRange UniformDescription::pushConstantRange() const {

	//The range to declare in the pipeline layout. A range of size 0 means there are no push constants.

	return data->pushConstantRange();
}

std::vector<VkDescriptorSetLayoutBinding> UniformDescription::bindings() const {

	//The shape of the descriptor set of the object to create a pipeline with.

	if (data->bufferBlock().empty()) return {};
	return { makeBinding() };
}

VkDescriptorSetLayoutBinding UniformDescription::makeBinding() const {

	VkDescriptorSetLayoutBinding binding{};
	binding.binding = BUFFER_BINDING;
	binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	binding.descriptorCount = 1;
	binding.stageFlags = data->bufferStages();
	binding.pImmutableSamplers = nullptr;

	return binding;
}