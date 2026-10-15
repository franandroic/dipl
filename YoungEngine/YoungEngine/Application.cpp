#include "Application.hpp"

Application::Application() : myWindow(WIDTH, HEIGHT, "Vulkan") {}

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

	myCanvas = std::make_unique<Canvas>(*mySwapChain, pixels, texWidth, texHeight, texChannels);

	myPipeline = std::make_unique<Pipeline>(*myDevice, myCanvas->renderPass, myCanvas->description);

	myLoader.unloadImage(pixels);
	
	myModelLoader.load(vertices, indices);

	myMesh = std::make_unique<Mesh>(*myDevice, vertices, indices);

	myCommand.createCommandBuffers(*myDevice);

	myRenderer = std::make_unique<MeshRenderer>(
		*myDevice,
		*mySwapChain,
		myWindow,
		*myCanvas,
		*myPipeline,
		*myMesh,
		myCommand,
		static_cast<uint32_t>(indices.size()),
		MAX_FRAMES_IN_FLIGHT
	);
}

void Application::mainLoop() {

	while (!glfwWindowShouldClose(myWindow.window)) {
		glfwPollEvents();
		myRenderer->render(myWindow.framebufferResized);
		myWindow.framebufferResized = false;
	}

	vkDeviceWaitIdle(myDevice->logical);
}
