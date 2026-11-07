#pragma once
#include "GLFWConfig.hpp"

#include <memory>

#include "DeviceUtility.hpp"
#include "ImageUtility.hpp"

#include "Device.hpp"
#include "Texture.hpp"
#include "DepthImageObject.hpp"
#include "RenderTarget.hpp"

class TextureRenderTarget : public RenderTarget {

public:

	//Construction
	TextureRenderTarget(VkExtent2D inExtent, VkFormat format, bool depth);
	~TextureRenderTarget() override = default;

	PipelineDescription::RenderingFormats getRenderingFormats() const override;
	VkExtent2D getExtent() const override { return texture.getExtent(); }
	const Texture &getTexture() const { return texture; }

	void recordBegin(VkCommandBuffer commandBuffer) const override;
	void recordEnd(VkCommandBuffer commandBuffer) const override;

	static PipelineDescription::RenderingFormats formatsFor(VkFormat format, bool depth);

private:

	VkFormat colorFormat;
	Texture texture;

	std::unique_ptr<DepthImageObject> depthImage;
};