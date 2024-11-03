/* Start Header
*****************************************************************/
/*!
\file       Input.cpp
\author     Hurng Kai Rui, h.kairui, 2301278
\par        email: h.kairui\@digipen.edu
\date       Sept 23, 2024
\brief      This file implements input handling functions for the Ukemochi
            Engine, providing functionalities to check key and mouse button 
            states, and retrieve mouse position.


Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "PreCompile.h"
#include "../Application.h"
#include "Input.h"
#include <GLFW/glfw3.h>

namespace Ukemochi {

	static std::unordered_map<int, bool> keyPressedMap; ///< Map to track key press states
	/*!
	\brief Checks if a key is currently pressed.
	\param Keycode The key to check.
	\return True if the key is pressed, false otherwise.
	*/
	bool Input::IsKeyPressed(int Keycode)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetKey(window, Keycode);
		return state == GLFW_PRESS;
	}

	/*!
	\brief Checks if a key is triggered (pressed for the first time).
	\param Keycode The key to check.
	\return True if the key is triggered, false otherwise.
	*/
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

	/*!
	\brief Checks if a mouse button is currently pressed.
	\param Mousecode The mouse button to check.
	\return True if the mouse button is pressed, false otherwise.
	*/
	bool Input::IsMouseButtonPressed(int Keycode)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetMouseButton(window, Keycode);
		return state == GLFW_PRESS;
	}

	/*!
	\brief Checks if a mouse button is triggered (pressed for the first time).
	\param Mousecode The mouse button code to check.
	\return True if the mouse button is triggered, false otherwise.
	*/
	bool Input::IsMouseButtonTriggered(int Keycode)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int state = glfwGetMouseButton(window, Keycode);

		if (state == GLFW_PRESS && !keyPressedMap[Keycode])
		{
			keyPressedMap[Keycode] = true;
			return true;  // Mouse button triggered for the first time
		}
		else if (state == GLFW_RELEASE)
		{
			keyPressedMap[Keycode] = false;
		}

		return false;  // Mouse button not triggered, or it's still held down
	}

	/*!
	\brief Gets the current mouse position.
	\return A pair containing the x and y coordinates of the mouse.
	*/
	std::pair<float, float> Input::GetMousePosition()
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return { static_cast<float>(xpos), static_cast<float>(ypos) };
	}

	/*!
	\brief Gets the current x coordinate of the mouse position.
	\return The x coordinate of the mouse.
	*/
	float Input::GetMouseX()
	{
		auto [x, y] = GetMousePosition();
		return x;
	}

	/*!
	\brief Gets the current y coordinate of the mouse position.
	\return The y coordinate of the mouse.
	*/
	float Input::GetMouseY()
	{
		auto [x, y] = GetMousePosition();
		return y;
	}

}