#pragma once
#include "GLFWConfig.hpp"

#include <cstddef>
#include <cstdint>

//Abstract strategy that defines what data an object passes to its shaders.
//By default it has no data at all: a concrete implementation overrides only the parts it needs.
//
//The data is split in two by the way it reaches the shaders:
//- the buffer block holds data that is updated often (typically every frame) and goes into a uniform buffer
//- the push constant block holds small data that is recorded straight into the command buffer
//
//The base class can't know the types of the data, so concrete implementations keep it
//in ordinary typed members and expose it through the accessors below.
class UniformData {

public:

	//A non-owning view of a piece of data that is passed to the shaders
	struct Block {
		const void *data = nullptr;
		size_t size = 0;

		bool empty() const { return data == nullptr || size == 0; }
	};

	virtual ~UniformData() = 0;

	virtual Block bufferBlock() const { return {}; }
	virtual Block pushConstantBlock() const { return {}; }

	//Shader stages that read the push constants. Used both when creating the pipeline layout
	//and when recording the push constants, which have to agree.
	virtual VkShaderStageFlags pushConstantStages() const { return 0; }

	//The range to declare in the pipeline layout. A range of size 0 means there are no push constants.
	VkPushConstantRange pushConstantRange() const {

		VkPushConstantRange range{};

		Block block = pushConstantBlock();
		if (block.empty()) return range;

		range.stageFlags = pushConstantStages();
		range.offset = 0;
		range.size = static_cast<uint32_t>(block.size);

		return range;
	}
};

inline UniformData::~UniformData() {}