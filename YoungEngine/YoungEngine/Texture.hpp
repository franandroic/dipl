#pragma once
#include "GLFWConfig.hpp"

#include <memory>
#include <vector>

#include "Device.hpp"
#include "Description.hpp"
#include "DescriptorSet.hpp"
#include "TextureImageObject.hpp"

//Everything that belongs to one texture instance: the image (with its mip chain and view),
//the sampler used to read it, its dimensions and the descriptor set the shaders read it through.
//A texture is sampled from and can be shared by multiple materials.
class Texture {

public:

	//Construction
	Texture(
		Device &inDevice,
		unsigned char *pixels,
		int inWidth, int inHeight, int inChannels
	);
	~Texture() = default;

	//Copy and Move constructors
	Texture(const Texture &) = delete;
	Texture &operator=(const Texture &) = delete;
	Texture(Texture &&) = delete;
	Texture &operator=(Texture &&) = delete;

	//Member variables
	const int width;
	const int height;
	const int channels;

	VkImageView getImageView() const { return TIO.imageView; }
	VkSampler getSampler() const { return TIO.sampler; }
	uint32_t getMipLevels() const { return TIO.mipLevels; }

	//The shape of the descriptor set of a texture (DescriptorTier::Material), to create a pipeline with.
	//Binding 0 is the combined image sampler, read in the fragment shader.
	static std::vector<VkDescriptorSetLayoutBinding> bindings();

	//Allocates the descriptor set pointing at the image and the sampler. The texture doesn't change
	//between frames, so a single set is shared by all the frames in flight.
	//The description has to be made from bindings(), which is the case for the one the pipeline
	//creates from them. Can be called once the pipeline exists.
	void createDescriptorSet(const Description &description);

	//Throws std::logic_error if the descriptor set hasn't been created
	const DescriptorSet &getSet() const;

private:
	
	Device &device;

	TextureImageObject TIO;
	std::unique_ptr<DescriptorSet> descriptorSet;
};