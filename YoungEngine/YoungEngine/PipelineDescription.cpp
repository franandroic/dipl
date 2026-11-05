#include "PipelineDescription.hpp"

namespace {

	//The Vulkan structs have no operator==, and nothing outside of this file needs to compare them.

	bool sameBindings(const std::vector<VkDescriptorSetLayoutBinding> &a, const std::vector<VkDescriptorSetLayoutBinding> &b) {

		if (a.size() != b.size()) return false;

		for (size_t i = 0; i < a.size(); i++) {
			if (a[i].binding != b[i].binding
			 || a[i].descriptorType != b[i].descriptorType
			 || a[i].descriptorCount != b[i].descriptorCount
			 || a[i].stageFlags != b[i].stageFlags) {
				return false;
			}
		}

		return true;
	}

	bool samePushConstantRange(const VkPushConstantRange &a, const VkPushConstantRange &b) {

		return a.stageFlags == b.stageFlags && a.offset == b.offset && a.size == b.size;
	}
}

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
		&& samePushConstantRange(pushConstantRange, other.pushConstantRange)
		&& sameBindings(materialBindings, other.materialBindings)
		&& sameBindings(objectBindings, other.objectBindings);
}