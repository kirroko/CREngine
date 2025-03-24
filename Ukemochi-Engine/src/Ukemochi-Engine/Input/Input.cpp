/* Start Header ************************************************************************/
/*!
\file       Input.cpp
\author     Hurng Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu
\date       Sept 23, 2024
\brief      This file implements input handling functions for the Ukemochi
            Engine, providing functionalities to check key and mouse button 
            states, and retrieve mouse position.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

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
		
		if (state == GLFW_RELEASE)
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

	/*!
	\brief Checks if a gamepad button is currently pressed.
	\param JoystickID The ID of the joystick (GLFW_JOYSTICK_1 through GLFW_JOYSTICK_16).
	\param ButtonID The button ID to check.
	\return True if the button is pressed, false otherwise.
	*/
	bool Input::IsGamepadButtonPressed(int JoystickID, int ButtonID)
	{
		if (!glfwJoystickIsGamepad(JoystickID))
		{
			UME_ENGINE_ERROR("Joystick is not a gamepad.");
			return false;
		}

		GLFWgamepadstate state;
		if (glfwGetGamepadState(JoystickID,&state) && ButtonID >= 0 && ButtonID <= GLFW_GAMEPAD_BUTTON_LAST)
		{
			return state.buttons[ButtonID] == GLFW_PRESS;
		}

		return false;
	}

	/*!
	\brief Checks if a gamepad button is triggered (pressed for the first time).
	\param JoystickID The ID of the joystick (GLFW_JOYSTICK_1 through GLFW_JOYSTICK_16).
	\param ButtonID The button ID to check.
	\return True if the button is triggered, false otherwise.
	*/
	bool Input::IsGamepadButtonTriggered(int JoystickID, int ButtonID)
	{
		if (!glfwJoystickIsGamepad(JoystickID))
		{
			UME_ENGINE_ERROR("Joystick is not a gamepad.");
			return false;
		}

		GLFWgamepadstate state;
		int uniqueKey = (JoystickID << 16) | ButtonID;
		if (glfwGetGamepadState(JoystickID,&state) && ButtonID >=0 && ButtonID <= GLFW_GAMEPAD_BUTTON_LAST)
		{
			if (state.buttons[ButtonID] == GLFW_PRESS && !keyPressedMap[uniqueKey])
            {
                keyPressedMap[uniqueKey] = true;
                return true;
            }
		}

		if (state.buttons[ButtonID] == GLFW_RELEASE)
		{
			keyPressedMap[uniqueKey] = false;
		}
		
		return false;
	}

	/*!
	\brief Gets the joystick axes values.
	\param JoystickID The ID of the joystick (GLFW_JOYSTICK_1 through GLFW_JOYSTICK_16).
	\param deadzone The deadzone value for the joystick axes.
	\return A vector of float values representing joystick axis positions, or empty if joystick is not present.
	*/
	std::vector<float> Input::GetJoystickAxes(int JoystickID, float deadzone)
	{
		// Clamp deadzone to valid range [0.0, 1.0]
		deadzone = std::max(0.0f,std::min(deadzone,1.0f));
		
		int axesCount;
		const float* axes = glfwGetJoystickAxes(JoystickID, &axesCount);
    
		if (axes == nullptr)
		{
			// Joystick not present or disconnected
			UME_ENGINE_TRACE("Joystick not present or disconnected.");
			return {};
		}

		// Convert the raw pointer to a vector with deadzone applied
		std::vector<float> processedAxes;
		processedAxes.reserve(axesCount);

		for (int i = 0; i < axesCount; ++i)
		{
			float value = axes[i];

			// Apply deadzone
			if (std::abs(value) < deadzone)
			{
				value = 0.0f;
			}
			else
			{
				// Rescale the values outside deadzone to full range
				// This creates a smooth transition from deadzone to max value
				float sign = (value > 0.0f) ? 1.0f : -1.0f;
				value = sign * (std::abs(value) - deadzone) / (1.0f - deadzone);
			}

			processedAxes.push_back(value);
		}
		
		return processedAxes;
	}
}
