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

	myPipelineCache = std::make_unique<PipelineCache>(*myDevice);

	myCamera = std::make_unique<Camera>(*myDevice, MAX_FRAMES_IN_FLIGHT);
	myCamera->setViewportExtent(mySwapChain->swapChainExtent.width, mySwapChain->swapChainExtent.height);

	myRenderer = std::make_unique<Renderer>(
		*myDevice,
		*mySwapChain,
		myWindow,
		*myCamera,
		MAX_FRAMES_IN_FLIGHT
	);

	myTexture = std::make_unique<Texture>(*myDevice, pixels, texWidth, texHeight, texChannels);
	myLoader.unloadImage(pixels);

	myModelLoader.load(vertices, indices);
	auto myMesh = std::make_unique<Mesh>(*myDevice, vertices, indices);

	auto myMaterial = std::make_unique<Material>(
		*myDevice,
		PipelineDescription::ShaderPair{ VERT_SHADER_PATH, FRAG_SHADER_PATH },
		std::make_unique<SimpleModelUD>(),
		std::make_unique<InPlaceYRotationUO>(90.0f),
		myTexture.get()
	);

	myRenderer->addDrawer(std::make_unique<GameObject>(
		*myPipelineCache,
		*myCamera,
		myRenderer->getRenderTarget().renderingFormats(),
		std::move(myMesh),
		std::move(myMaterial)
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
