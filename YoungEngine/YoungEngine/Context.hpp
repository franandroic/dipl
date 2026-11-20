#pragma once
#include "GLMConfig.hpp"

#include <cassert>

//We're going to use forward declarations here because we're only holding pointers
//to objects. We won't be using them in any way so we don't need to include the files
//and considering we want everyone to have access to context, we want to avoid cyclic dependency
class Scene;

//Used as a globally-accessible state storage
class Context {

public:

	//Construction
	Context() {
		assert(!active && "Context already exists!");
		active = this;
	}

	~Context() {
		if (active == this) active = nullptr;
	}

	//Copy and Move constructors
	Context(const Context &) = delete;
	Context &operator=(const Context &) = delete;
	Context(Context &&) = delete;
	Context &operator=(Context &&) = delete;

	//Global access
	static inline Context *active = nullptr;

	//World's coordinate system
	static constexpr glm::vec3 GLOBAL_RIGHT{ 1.0f, 0.0f, 0.0f };
	static constexpr glm::vec3 GLOBAL_FORWARD{ 0.0f, 0.0f, -1.0f };
	static constexpr glm::vec3 GLOBAL_UP{ 0.0f, 1.0f, 0.0f };

	//Camera's coordinate system
	static constexpr glm::vec3 CAMERA_RIGHT{ 1.0f, 0.0f, 0.0f };
	static constexpr glm::vec3 CAMERA_FORWARD{ 0.0f, 0.0f, 1.0f };
	static constexpr glm::vec3 CAMERA_UP{ 0.0f, 1.0f, 0.0f };

	//Engine's objects that may be active one at a time
	Scene *activeScene = nullptr;
};

//We want to enforce right-handedness of the global coordinate system
static_assert(
	glm::cross(Context::GLOBAL_FORWARD, Context::GLOBAL_UP) == Context::GLOBAL_RIGHT,
	"The global axes have to be right-handed"
);