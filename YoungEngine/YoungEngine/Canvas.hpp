#pragma once
#include "GLFWConfig.hpp"

#include <vector>
#include <memory>

#include "DeviceData.hpp"

#include "Device.hpp"
#include "Description.hpp"
#include "Texture.hpp"
#include "UniformBufferObject.hpp"
#include "UniformData.hpp"
#include "UniformOperator.hpp"
#include "Uniformity.hpp"
#include "Camera.hpp"

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
	~Canvas();

	//Copy and Move constructors
	Canvas(const Canvas &) = delete;
	Canvas &operator=(const Canvas &) = delete;
	Canvas(Canvas &&) = delete;
	Canvas &operator=(Canvas &&) = delete;

	//Member variables
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	Description description;

	Texture texture;

	//The camera's uniform buffers, one for each frame in flight.
	//They're shared by all objects, so they're temporarily here until the descriptors are reworked.
	std::vector<std::unique_ptr<UniformBufferObject>> UBOs;

	//The uniforms of the object. Canvas stands in for the future GameObject, which will own this.
	Uniformity uniformity;

private:

	Device &device;

	void createDescriptorPool();
};