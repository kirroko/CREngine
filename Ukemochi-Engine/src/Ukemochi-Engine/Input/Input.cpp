#include "PreCompile.h"
#include "../Application.h"
#include "Input.h"
#include <GLFW/glfw3.h>

namespace UME {

	static std::unordered_map<int, bool> keyPressedMap;

	bool Input::IsKeyPressed(int Keycode)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetKey(window, Keycode);
		return state == GLFW_PRESS;
	}

	bool Input::IsKeyTriggered(int Keycode)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetKey(window, Keycode);

		if (state == GLFW_PRESS && !keyPressedMap[Keycode])
		{
			keyPressedMap[Keycode] = true;
			return true;  // Key triggered for the first time
		}
		else if (state == GLFW_RELEASE)
		{
			keyPressedMap[Keycode] = false;
		}

		return false;  // Key not triggered, or it's still held down
	}

	bool Input::IsMouseButtonPressed(int Keycode)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetMouseButton(window, Keycode);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> Input::GetMousePosition()
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return { static_cast<float>(xpos), static_cast<float>(ypos) };
	}

	float Input::GetMouseX()
	{
		auto [x, y] = GetMousePosition();
		return x;
	}

	float Input::GetMouseY()
	{
		auto [x, y] = GetMousePosition();
		return y;
	}
}