#include "Canvas.hpp"

Canvas::Canvas(
	Device &inDevice,
	unsigned char *pixels,
	int texWidth, int texHeight, int texChannels,
	std::unique_ptr<UniformData> inUniformData,
	std::unique_ptr<UniformOperator> inUniformOperator
) : texture(inDevice, pixels, texWidth, texHeight, texChannels),
	uniformity(inDevice, std::move(inUniformData), std::move(inUniformOperator), MAX_FRAMES_IN_FLIGHT) {
}