#include "FrameBufferObject.hpp"

FrameBufferObject::FrameBufferObject(SwapChain *inSwapChain, RenderPass *renderPass, unsigned char *pixels, int texWidth, int texHeight, int texChannels) {

	swapChain = inSwapChain;

	CIO = ColorImageObject(swapChain->device, swapChain->swapChainImageFormat);
	CIO.createImage(swapChain->swapChainExtent.width, swapChain->swapChainExtent.height);

	DIO = DepthImageObject(swapChain->device, DeviceUtils::findDepthFormat(swapChain->device->physical));
	DIO.createImage(swapChain->device->commandPool, swapChain->swapChainExtent.width, swapChain->swapChainExtent.height);

	TIO = TextureImageObject(swapChain->device);
	TIO.createImage(swapChain->device->commandPool, pixels, texWidth, texHeight, texChannels);
	TIO.createTextureSampler();

	createFramebuffers(renderPass);
}

void FrameBufferObject::createFramebuffers(RenderPass *renderPass) {

	//Frame buffers need references to swap chain images (accessed through image views)
	//provided in the form attachments. We specify the info details, create the
	//frame buffers and store the handle to them.

	framebuffers.resize(swapChain->swapChainImageViews.size());

	for (size_t i = 0; i < swapChain->swapChainImageViews.size(); i++) {

		std::array<VkImageView, 3> attachments = { CIO.imageView, DIO.imageView, swapChain->swapChainImageViews[i] };

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass->renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChain->swapChainExtent.width;
		framebufferInfo.height = swapChain->swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(swapChain->device->logical, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create framebuffer!");
		}
	}
}

void FrameBufferObject::recreateFramebuffers(RenderPass *renderPass) {

	CIO = ColorImageObject(swapChain->device, swapChain->swapChainImageFormat);
	CIO.createImage(swapChain->swapChainExtent.width, swapChain->swapChainExtent.height);

	DIO = DepthImageObject(swapChain->device, DeviceUtils::findDepthFormat(swapChain->device->physical));
	DIO.createImage(swapChain->device->commandPool, swapChain->swapChainExtent.width, swapChain->swapChainExtent.height);

	createFramebuffers(renderPass);
}