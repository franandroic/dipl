#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "Engine.hpp"

int main() {

	Engine engine;

	try { 
		engine.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}