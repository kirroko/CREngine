#pragma once
#include "KeyCode.h"
#include <PreCompile.h>

namespace UME {

	class Input
	{
	public:
		static bool IsKeyPressed(int Keycode);
		static bool IsMouseButtonPressed(int Mousecode);
		static std::pair<float,float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};

}