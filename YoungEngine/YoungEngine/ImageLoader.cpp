#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "ImageLoader.hpp"

ImageLoader::ImageLoader() {

	path = "";
}

unsigned char *ImageLoader::load(int *width, int *height, int *channels) {

	return stbi_load(path.c_str(), width, height, channels, STBI_rgb_alpha);
}

void ImageLoader::unload(unsigned char *image) {

	stbi_image_free(image);
}

void ImageLoader::setPath(std::string inPath) {

	path = inPath;
}