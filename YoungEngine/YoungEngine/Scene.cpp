#include "Scene.hpp"

void Scene::add(std::unique_ptr<Renderable> renderable) {

	if (!renderable) {
		throw std::invalid_argument("Can't add an empty renderable to a scene!");
	}

	renderables.push_back(std::move(renderable));
}

void Scene::update(float dt) {

	for (auto &renderable : renderables) {
		renderable->update(dt);
	}
}
