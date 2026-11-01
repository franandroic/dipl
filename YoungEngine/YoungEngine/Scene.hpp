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

	void add(std::unique_ptr<Renderable> renderable);
	void update(float dt);

	Camera &getCamera() { return camera; }
	const std::vector<std::unique_ptr<Renderable>> &getRenderables() const { return renderables; }

private:

	Camera camera;
	std::vector<std::unique_ptr<Renderable>> renderables;
};