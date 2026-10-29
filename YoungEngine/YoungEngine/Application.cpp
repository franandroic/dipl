#include "Application.hpp"

Application::Application() :
	myModelLoader(),
	myImageLoader(),
	myLoader(),
	myWindow(WIDTH, HEIGHT, "Vulkan"),
	myEngineUtility() {
}

void Application::run() {

	initVulkan();
	mainLoop();
}

void Application::initVulkan() {

	myLoader.setModelLoader(&myModelLoader);
	myLoader.setImageLoader(&myImageLoader);

	myDevice = std::make_unique<Device>(myEngineUtility.instance, myWindow.window);

	mySwapChain = std::make_unique<SwapChain>(*myDevice, myWindow.window);
	mySwapChain->createImageViews();

	myPipelineCache = std::make_unique<PipelineCache>(*myDevice);

	myScene = std::make_unique<Scene>();

	Camera &camera = myScene->getCamera();
	camera.position = glm::vec3(2.0f, -6.0f, 4.0f);
	camera.target = glm::vec3(0.0f, 1.5f, 0.0f);
	camera.farPlane = 50.0f;

	myRenderer = std::make_unique<Renderer>(
		*myDevice,
		*mySwapChain,
		myWindow,
		*myPipelineCache,
		*myScene,
		MAX_FRAMES_IN_FLIGHT
	);

	//FIRST OBJECT: first mesh, material of the first texture and the first shader pair
	Mesh &firstMesh = loadMesh(MODEL_PATH);
	Texture &firstTexture = loadTexture(FIRST_TEXTURE_PATH);
	Material &firstMaterial = createMaterial(TEXTURED_SHADERS, &firstTexture);

	myScene->add(std::make_unique<GameObject>(
		*myDevice,
		firstMesh,
		firstMaterial,
		std::make_unique<SimpleModelUD>(glm::vec3(-3.0f, 3.0f, 0.0f)),
		std::make_unique<InPlaceYRotationUO>(90.0f)
	));

	//SECOND OBJECT: first mesh, material of the second texture and the first shader pair
	Texture &secondTexture = loadTexture(SECOND_TEXTURE_PATH);
	Material &secondMaterial = createMaterial(TEXTURED_SHADERS, &secondTexture);

	myScene->add(std::make_unique<GameObject>(
		*myDevice,
		firstMesh,
		secondMaterial,
		std::make_unique<SimpleModelUD>(glm::vec3(0.0f, 3.0f, 0.0f)),
		std::make_unique<InPlaceYRotationUO>(-45.0f)
	));

	//THIRD OBJECT: first mesh, material without a texture and with the second shader pair
	Material &thirdMaterial = createMaterial(UNTEXTURED_SHADERS);

	myScene->add(std::make_unique<GameObject>(
		*myDevice,
		firstMesh,
		thirdMaterial,
		std::make_unique<SimpleModelUD>(glm::vec3(3.0f, 3.0f, 0.0f)),
		std::make_unique<InPlaceYRotationUO>(30.0f)
	));

	//THE AXES OF THE WORLD, at the origin
	myScene->add(std::make_unique<Axes>(*myDevice, AXES_SHADERS, 1.5f));
}

void Application::mainLoop() {

	auto previousTime = std::chrono::steady_clock::now();

	while (!glfwWindowShouldClose(myWindow.window)) {

		glfwPollEvents();
		
		auto currentTime = std::chrono::steady_clock::now();
		float dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - previousTime).count();
		previousTime = currentTime;

		myScene->update(dt);
		
		myRenderer->render(myWindow.framebufferResized);
		myWindow.framebufferResized = false;
	}

	vkDeviceWaitIdle(myDevice->logical);
}

Mesh &Application::loadMesh(const std::string &path) {

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	myModelLoader.setPath(path);
	myLoader.loadModel(vertices, indices);

	myMeshes.push_back(std::make_unique<Mesh>(*myDevice, std::move(vertices), std::move(indices)));

	return *myMeshes.back();
}

Texture &Application::loadTexture(const std::string &path) {

	myImageLoader.setPath(path);

	int texWidth, texHeight, texChannels;
	unsigned char *pixels = myLoader.loadImage(&texWidth, &texHeight, &texChannels);
	if (!pixels) {
		throw std::runtime_error("Failed to load texture image: " + path);
	}

	//The pixels are only needed until they're uploaded, and have to be freed even if that throws
	try {
		myTextures.push_back(std::make_unique<Texture>(*myDevice, pixels, texWidth, texHeight, texChannels));
	} catch (...) {
		myLoader.unloadImage(pixels);
		throw;
	}

	myLoader.unloadImage(pixels);

	return *myTextures.back();
}

Material &Application::createMaterial(const PipelineDescription::ShaderPair &shaders, const Texture *texture) {

	myMaterials.push_back(std::make_unique<Material>(*myDevice, shaders, texture));

	return *myMaterials.back();
}