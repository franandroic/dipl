#include "Application.hpp"

Application::Application() :
	myModelLoader(),
	myImageLoader(),
	myLoader(),
	myWindow(WIDTH, HEIGHT, "Vulkan"),
	myEngineUtility(),
	myCamera() {
}

void Application::run() {

	initVulkan();
	mainLoop();
}

void Application::initVulkan() {

	myModelLoader.setPath(MODEL_PATH);
	myImageLoader.setPath(TEXTURE_PATH);
	myLoader.setModelLoader(&myModelLoader);
	myLoader.setImageLoader(&myImageLoader);

	int texWidth, texHeight, texChannels;
	unsigned char *pixels = myLoader.loadImage(&texWidth, &texHeight, &texChannels);
	if (!pixels) {
		throw std::runtime_error("Failed to load texture image!");
	}
	
	myDevice = std::make_unique<Device>(myEngineUtility.instance, myWindow.window);

	mySwapChain = std::make_unique<SwapChain>(*myDevice, myWindow.window);
	mySwapChain->createImageViews();

	myCamera = std::make_unique<Camera>(*myDevice, MAX_FRAMES_IN_FLIGHT);
	myCamera->setViewportExtent(mySwapChain->swapChainExtent.width, mySwapChain->swapChainExtent.height);

	myRenderer = std::make_unique<Renderer>(
		*myDevice,
		*mySwapChain,
		myWindow,
		*myCamera,
		MAX_FRAMES_IN_FLIGHT
	);

	myCanvas = std::make_unique<Canvas>(
		*myDevice,
		pixels, texWidth, texHeight, texChannels,
		std::make_unique<SimpleModelUD>(),
		std::make_unique<InPlaceYRotationUO>(90.0f)
	);

	std::vector<std::vector<VkDescriptorSetLayoutBinding>> setBindings(DescriptorTier::Count);
	setBindings[DescriptorTier::Frame] = Camera::bindings();
	setBindings[DescriptorTier::Material] = Texture::bindings();
	setBindings[DescriptorTier::Object] = myCanvas->uniformity.bindings();

	myPipeline = std::make_unique<Pipeline>(
		*myDevice,
		*mySwapChain,
		setBindings,
		myCanvas->uniformity.pushConstantRange()
	);

	myCamera->createDescriptorSets(myPipeline->getDescription(DescriptorTier::Frame));
	myCanvas->texture.createDescriptorSet(myPipeline->getDescription(DescriptorTier::Material));
	myCanvas->uniformity.createDescriptorSets(myPipeline->getDescription(DescriptorTier::Object));

	myLoader.unloadImage(pixels);
	myModelLoader.load(vertices, indices);

	myMesh = std::make_unique<Mesh>(*myDevice, vertices, indices);

	myRenderer->addDrawer(std::make_unique<MeshDrawer>(
		*myCanvas,
		*myPipeline,
		*myMesh,
		static_cast<uint32_t>(indices.size()),
		*myCamera
	));
}

void Application::mainLoop() {

	while (!glfwWindowShouldClose(myWindow.window)) {
		glfwPollEvents();
		myRenderer->render(myWindow.framebufferResized);
		myWindow.framebufferResized = false;
	}

	vkDeviceWaitIdle(myDevice->logical);
}
