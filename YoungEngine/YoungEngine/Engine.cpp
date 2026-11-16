#include "Engine.hpp"

Engine::Engine() :
	myModelLoader(),
	myImageLoader(),
	myLoader(),
	myWindow(WIDTH, HEIGHT, "Vulkan"),
	myEngineUtility() {
}

void Engine::run() {

	initialize();
	renderLoop();
}

void Engine::initialize() {

	myLoader.setModelLoader(&myModelLoader);
	myLoader.setImageLoader(&myImageLoader);

	myDevice = std::make_unique<Device>(myEngineUtility.instance, myWindow.window);

	mySwapChain = std::make_unique<SwapChain>(myWindow.window);
	mySwapChain->createImageViews();

	myPipelineCache = std::make_unique<PipelineCache>();

	myScene = std::make_unique<Scene>();

	Camera &camera = myScene->getCamera();
	camera.position = glm::vec3(2.0f, -6.0f, 4.0f);
	camera.target = glm::vec3(0.0f, 1.5f, 0.0f);
	camera.farPlane = 50.0f;

	LightSource &light = myScene->getLight();
	light.position = glm::vec3(0.0f, 0.0f, 4.0f);
	light.intensity = 1.5f;

	myRenderer = std::make_unique<Renderer>(
		myWindow,
		*myScene,
		MAX_FRAMES_IN_FLIGHT
	);

	//THE AXES OF THE WORLD, at the origin
	myScene->add(std::make_unique<Axes>(AXES_SHADERS, 100.0f));

	//FIRST OBJECT: first mesh, material of the first texture and the first shader pair
	Mesh &firstMesh = loadMesh(MODEL_PATH);

	MaterialPass::Settings normalPass;
	normalPass.shaders = NORMAL_SHADERS;
	normalPass.rendersOffscreen = true;
	normalPass.offscreenFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
	normalPass.offscreenDepth = true;

	MaterialPass::Settings colorPass;
	colorPass.shaders = PHONG_SHADERS;
	colorPass.rendersOffscreen = true;
	colorPass.offscreenDepth = true;

	MaterialPass::Settings edgePass;
	edgePass.shaders = SKETCH_SHADERS;
	edgePass.passInputs = { 0, 1 };
	edgePass.hasFullscreenContext = true;
	edgePass.raster.cullMode = VK_CULL_MODE_NONE;
	edgePass.raster.depthTest = false;
	edgePass.raster.depthWrite = false;
	edgePass.raster.blend = PipelineDescription::BlendMode::Alpha;

	Material &sketchMaterial = createMaterial({ normalPass, colorPass, edgePass });
	
	myScene->add(
		std::make_unique<Prop>(
			firstMesh,
			sketchMaterial,
			std::make_unique<SimpleLitUD>(light),
			std::make_unique<InPlaceYRotationUO>(5.0f, &light)
		)
	);
}

void Engine::renderLoop() {

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

	vkDeviceWaitIdle(Device::active->logical);
}

Mesh &Engine::loadMesh(const std::string &path) {

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	myModelLoader.setPath(path);
	myLoader.loadModel(vertices, indices);

	myMeshes.push_back(std::make_unique<Mesh>(std::move(vertices), std::move(indices)));

	return *myMeshes.back();
}

Texture &Engine::loadTexture(const std::string &path) {

	myImageLoader.setPath(path);

	int texWidth, texHeight, texChannels;
	unsigned char *pixels = myLoader.loadImage(&texWidth, &texHeight, &texChannels);
	if (!pixels) {
		throw std::runtime_error("Failed to load texture image: " + path);
	}

	//The pixels are only needed until they're uploaded, and have to be freed even if that throws
	try {
		myTextures.push_back(std::make_unique<Texture>(pixels, texWidth, texHeight, texChannels));
	} catch (...) {
		myLoader.unloadImage(pixels);
		throw;
	}

	myLoader.unloadImage(pixels);

	return *myTextures.back();
}

Material &Engine::createMaterial(const PipelineDescription::ShaderPair &shaders, const Texture *texture) {

	myMaterials.push_back(std::make_unique<Material>(shaders, texture));

	return *myMaterials.back();
}

Material &Engine::createMaterial(const std::vector<MaterialPass::Settings> &passes) {

	myMaterials.push_back(std::make_unique<Material>(passes));

	return *myMaterials.back();
}