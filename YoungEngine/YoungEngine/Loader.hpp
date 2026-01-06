#pragma once

#include <vector>

#include "ModelLoader.hpp"
#include "ImageLoader.hpp"
#include "Vertex.hpp"

class Loader {

public:

	Loader() = default;

	void loadModel(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices) { modelLoader->load(vertices, indices); }

	unsigned char *loadImage(int *width, int *height, int *channels) { return imageLoader->load(width, height, channels); }

	void unloadImage(unsigned char *image) { imageLoader->unload(image); }

	void setModelLoader(ModelLoader *inModelLoader) { modelLoader = inModelLoader; }

	void setImageLoader(ImageLoader *inImageLoader) { imageLoader = inImageLoader; }

private:

	ModelLoader *modelLoader;

	ImageLoader *imageLoader;
};