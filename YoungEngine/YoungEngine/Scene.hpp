#pragma once

#include <memory>
#include <stdexcept>
#include <vector>

#include "Camera.hpp"
#include "Renderable.hpp"

//What there is to see: the renderables, which the scene owns, and the camera they're seen through.
class Scene {

public:

	//Construction
	Scene() = default;
	~Scene() = default;

	//Copy and Move constructors
	Scene(const Scene &) = delete;
	Scene &operator=(const Scene &) = delete;
	Scene(Scene &&) = delete;
	Scene &operator=(Scene &&) = delete;

	Camera &getCamera() { return camera; }
	const Camera &getCamera() const { return camera; }

	//Takes ownership of the renderable and returns it, to keep setting it up through its own type.
	//TODO: Removing. The GPU might still be reading a renderable's buffers for the frames in flight,
	//so its destruction has to be deferred until those frames are done.
	void add(std::unique_ptr<Renderable> renderable);

	const std::vector<std::unique_ptr<Renderable>> &getRenderables() const { return renderables; }

	//Advances the logic of everything in the scene by dt seconds
	void update(float dt);

private:

	Camera camera;
	std::vector<std::unique_ptr<Renderable>> renderables;
};