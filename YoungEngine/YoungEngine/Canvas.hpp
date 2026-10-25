#pragma once
#include "GLFWConfig.hpp"

#include <memory>

#include "DeviceData.hpp"

#include "Device.hpp"
#include "Description.hpp"
#include "Texture.hpp"
#include "UniformData.hpp"
#include "UniformOperator.hpp"
#include "Uniformity.hpp"

//Contains objects used to define the look of an object
class Canvas {

public:

	//Construction
	Canvas(
		Device &inDevice,
		unsigned char *pixels,
		int texWidth, int texHeight, int texChannels,
		std::unique_ptr<UniformData> inUniformData,
		std::unique_ptr<UniformOperator> inUniformOperator
	);
	~Canvas() = default;

	//Copy and Move constructors
	Canvas(const Canvas &) = delete;
	Canvas &operator=(const Canvas &) = delete;
	Canvas(Canvas &&) = delete;
	Canvas &operator=(Canvas &&) = delete;

	//Member variables
	Texture texture;

	//The uniforms of the object. Canvas stands in for the future GameObject, which will own this.
	Uniformity uniformity;
};