#pragma once
#include "GLFWConfig.hpp"

#include "UniformData.hpp"

//Abstract strategy for the logic that updates an object's uniform data every frame.
//A concrete operator works with a specific kind of UniformData and is paired with it in a Uniformity.
class UniformOperator {

public:

	virtual ~UniformOperator() = default;

	//Called once per frame.
	virtual void operate(UniformData &data) = 0;
};