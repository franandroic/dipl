#include "PipelineDescription.hpp"

std::vector<VkDescriptorSetLayoutBinding> PipelineDescription::frameBindings() {

	//Visible to the fragment stage as well, so a fragment shader can light with the camera's position

	VkDescriptorSetLayoutBinding cameraBinding{};
	cameraBinding.binding = FRAME_CAMERA_BINDING;
	cameraBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	cameraBinding.descriptorCount = 1;
	cameraBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	cameraBinding.pImmutableSamplers = nullptr;

	return { cameraBinding };
}

bool PipelineDescription::ShaderPair::operator==(const ShaderPair &other) const {

	return vertex == other.vertex && fragment == other.fragment;
}

bool PipelineDescription::RenderingFormats::operator==(const RenderingFormats &other) const {

	return colorFormat == other.colorFormat && depthFormat == other.depthFormat && samples == other.samples;
}

bool PipelineDescription::RasterState::operator==(const RasterState &other) const {

	return cullMode == other.cullMode
		&& depthTest == other.depthTest
		&& depthWrite == other.depthWrite
		&& blend == other.blend;
}

bool PipelineDescription::operator==(const PipelineDescription &other) const {

	return shaders == other.shaders
		&& formats == other.formats
		&& raster == other.raster
		&& vertexInput == other.vertexInput
		&& topology == other.topology
		&& RenderUtility::samePushConstantRange(pushConstantRange, other.pushConstantRange)
		&& RenderUtility::sameBindings(materialBindings, other.materialBindings)
		&& RenderUtility::sameBindings(objectBindings, other.objectBindings);
}