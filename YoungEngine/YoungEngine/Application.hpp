#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include <iostream>

class Application {

public:

	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

private:

	GLFWwindow *window;
	VkInstance instance;

public:

	void run();

private:
	
	//RUNTIME FUNCTIONS
	void initWindow();

	void initVulkan();

	void mainLoop();

	void cleanup();

	//SUPPORT FUNCTIONS
	void createInstance();

	void enableRequiredExtensions(VkInstanceCreateInfo &info, bool verbose);

};