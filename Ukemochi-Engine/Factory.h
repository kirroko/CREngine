#include "src/Ukemochi-Engine/Serialization/Serialization.h"
#include "PreCompile.h"

#pragma once

struct GameObj
{
	std::string type;
	int x_val;
	int y_val;
	int width;
	int height;
	int radius;
	int segment;
};

namespace Factory
{
	void CloneObject(int count, const std::string& type);
};