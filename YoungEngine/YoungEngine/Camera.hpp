#pragma once
#include "GLFWConfig.hpp"
#include "GLMConfig.hpp"

#include <cstdint>
#include <memory>
#include <vector>

#include "Device.hpp"
#include "DescriptorSet.hpp"
#include "UniformBufferObject.hpp"

//Describes the point of view the scene is rendered from.
class Camera {

public:

	//The data the camera passes to the shaders, laid out to match the uniform block in the vertex shader.
	//It changes once per frame and is shared by every object drawn in it.
	struct UniformBlock {
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
		alignas(16) glm::vec3 position;
	};

	//Construction
	Camera(Device &inDevice, size_t framesInFlight);
	~Camera() = default;

	//Copy and Move constructors
	Camera(const Camera &) = delete;
	Camera &operator=(const Camera &) = delete;
	Camera(Camera &&) = delete;
	Camera &operator=(Camera &&) = delete;

	//Member variables
	glm::vec3 position{ 0.0f, -2.0f, 1.0f };
	glm::vec3 target{ 0.0f, 0.0f, 0.0f };
	glm::vec3 up{ 0.0f, 0.0f, 1.0f };
	float fovDegrees = 45.0f;
	float nearPlane = 0.1f;
	float farPlane = 10.0f;

	void setViewportExtent(uint32_t width, uint32_t height);

	UniformBlock getUniformBlock() const;

	//Uploads the current state of the camera into the uniform buffer of the frame.
	//Called once per frame, after the GPU has finished with the resources of this frame in flight.
	void update(size_t frame);

	//Throws std::out_of_range if the descriptor sets haven't been created
	const DescriptorSet &getSet(size_t frame) const { return *descriptorSets.at(frame); }

private:

	//Binding 0 is the uniform block, read in the vertex shader. Pipelines assume exactly this
	//for their first set (see Pipeline), so it can't be changed without changing them.
	static constexpr uint32_t BUFFER_BINDING = 0;
	static VkDescriptorSetLayoutBinding makeBinding();

	float aspectRatio = 1.0f;

	Device &device;

	std::vector<std::unique_ptr<UniformBufferObject>> UBOs;
	std::vector<std::unique_ptr<DescriptorSet>> descriptorSets;
};