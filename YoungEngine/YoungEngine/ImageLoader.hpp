#pragma once

#include <string>

class ImageLoader {

	//TODO: Make this an abstract class with concrete implementations to pass to Loader

public:

	ImageLoader();

	unsigned char *load(int *width, int *height, int *channels);

	void unload(unsigned char *image);

	void setPath(std::string inPath);

private:

	std::string path;

};