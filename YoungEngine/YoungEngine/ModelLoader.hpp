#pragma once

#include <unordered_map>
#include <stdexcept>
#include <string>

#include "Vertex.hpp"

class ModelLoader {

	//TODO: Make this an abstract class with concrete implementations to pass to Loader

public:

	ModelLoader();

	void load(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices);

	void setPath(std::string inPath);

private:

	std::string path;

};