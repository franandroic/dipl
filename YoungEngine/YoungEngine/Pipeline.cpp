#include "Pipeline.hpp"

Pipeline::Pipeline(const PipelineDescription &inDescription) : description(inDescription) {

	VkShaderModule vertShaderModule = VK_NULL_HANDLE;
	VkShaderModule fragShaderModule = VK_NULL_HANDLE;

	try {

		auto vertShaderCode = readFile(description.shaders.vertex);
		auto fragShaderCode = readFile(description.shaders.fragment);

		//Printing the size of the read code to compare with its file size of disk.
		//std::cout << "SHADER SIZES: " << vertShaderCode.size() << " " << fragShaderCode.size() << std::endl;

		vertShaderModule = createShaderModule(vertShaderCode);
		fragShaderModule = createShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		//TODO: Look into combining multiple shaders into a single module with different entry points
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";
		//TODO: Look into the pSpecializationInfo member of the info struct

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		VkVertexInputBindingDescription bindingDescription = Vertex::getBindingDescription();
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = Vertex::getAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = description.topology;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
		//TODO: Look into using primitiveRestartEnable and different topologies

		//Commenting this part out because of a later setup for dynamic control of these objects
		/*
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapChainExtent.width;
		viewport.height = (float)swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = swapChainExtent;
		*/

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		//viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		//viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;
		//TODO: Play with these settings. NOTE: check which require enabling a GPU feature.

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = Device::active->msaaSamples;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;
		//TODO: Look into sample shading

		bool hasColor = description.formats.hasColor();
		bool hasDepth = description.formats.hasDepth();

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = hasDepth ? VK_TRUE : VK_FALSE;
		depthStencil.depthWriteEnable = hasDepth ? VK_TRUE : VK_FALSE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f;
		depthStencil.maxDepthBounds = 1.0f;
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {};
		depthStencil.back = {};
		//TODO: Add stencil buffers

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = hasColor ? 1 : 0;
		colorBlending.pAttachments = hasColor ? &colorBlendAttachment : nullptr;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;
		//TODO: Look into color blending and how it's useful.

		std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		//Creating the layout of each set, in the order of the tiers (the position in the list is the set number).
		//The first one is the Frame set, which is the same for every pipeline and filled by the renderer.
		setLayouts.resize(PipelineDescription::DescriptorTier::Count);
		//set = 0
		setLayouts[PipelineDescription::DescriptorTier::Frame] = createSetLayout(PipelineDescription::frameBindings());
		//set = 1
		setLayouts[PipelineDescription::DescriptorTier::Material] = createSetLayout(description.materialBindings);
		//set = 2
		setLayouts[PipelineDescription::DescriptorTier::Object] = createSetLayout(description.objectBindings);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
		pipelineLayoutInfo.pSetLayouts = setLayouts.data();
		//A range of size zero means the pipeline uses no push constants.
		bool usesPushConstants = description.pushConstantRange.size > 0;
		pipelineLayoutInfo.pushConstantRangeCount = usesPushConstants ? 1 : 0;
		pipelineLayoutInfo.pPushConstantRanges = usesPushConstants ? &description.pushConstantRange : nullptr;

		if (vkCreatePipelineLayout(Device::active->logical, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout!");
		}

		//Dynamic rendering: instead of a render pass, we state which attachment formats the pipeline renders to.
		//These have to match the formats of the image views passed to vkCmdBeginRendering when the pipeline is used.
		VkFormat colorFormat = description.formats.colorFormat;
		VkFormat depthFormat = description.formats.depthFormat;

		VkPipelineRenderingCreateInfo renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		renderingInfo.viewMask = 0;
		renderingInfo.colorAttachmentCount = hasColor ? 1 : 0;
		renderingInfo.pColorAttachmentFormats = hasColor ? &colorFormat : nullptr;
		renderingInfo.depthAttachmentFormat = depthFormat;
		renderingInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pNext = &renderingInfo;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = VK_NULL_HANDLE;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(Device::active->logical, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create graphics pipeline!");
		}

		vkDestroyShaderModule(Device::active->logical, fragShaderModule, nullptr);
		vkDestroyShaderModule(Device::active->logical, vertShaderModule, nullptr);

	} catch (...) {

		//The destructor doesn't run for an object whose constructor threw,
		//so whatever was created up to the failure is destroyed here.
		vkDestroyShaderModule(Device::active->logical, fragShaderModule, nullptr);
		vkDestroyShaderModule(Device::active->logical, vertShaderModule, nullptr);
		destroy();

		throw;
	}
}

Pipeline::~Pipeline() {

	destroy();
}

void Pipeline::destroy() {

	vkDestroyPipeline(Device::active->logical, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(Device::active->logical, pipelineLayout, nullptr);

	for (VkDescriptorSetLayout setLayout : setLayouts) {
		vkDestroyDescriptorSetLayout(Device::active->logical, setLayout, nullptr);
	}

	graphicsPipeline = VK_NULL_HANDLE;
	pipelineLayout = VK_NULL_HANDLE;
	setLayouts.clear();
}

VkDescriptorSetLayout Pipeline::createSetLayout(const std::vector<VkDescriptorSetLayoutBinding> &bindings) {

	//Specifying what kind of descriptor sets we're going to bind to the pipeline, to which bindings
	//and to which shader stages.

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	VkDescriptorSetLayout setLayout;

	if (vkCreateDescriptorSetLayout(Device::active->logical, &layoutInfo, nullptr, &setLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor set layout!");
	}

	return setLayout;
}

VkShaderModule Pipeline::createShaderModule(const std::vector<char> &code) {

	//The bytes of code passed as the argument need to be forcibly cast to unit32_t
	//so they can be fed into the shader module create info. We return the created shader module.

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

	VkShaderModule shaderModule;

	if (vkCreateShaderModule(Device::active->logical, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create shader module!");
	}

	return shaderModule;
}

std::vector<char> Pipeline::readFile(const std::string &filename) {

	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + filename);
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}