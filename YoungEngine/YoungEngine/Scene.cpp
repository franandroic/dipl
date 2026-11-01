#include "Scene.hpp"

void Scene::add(std::unique_ptr<Renderable> renderable) {

	//Takes ownership of the renderable.
	//TODO: Removing. The GPU might still be reading a renderable's buffers for the frames in flight,
	//so its destruction has to be deferred until those frames are done.

	if (!renderable) {
		throw std::invalid_argument("Can't add an empty renderable to a scene!");
	}

	renderables.push_back(std::move(renderable));
}

void Scene::update(float dt) {

	//Advances the logic of everything in the scene by dt seconds

	for (auto &renderable : renderables) {
		renderable->update(dt);
	}
}
