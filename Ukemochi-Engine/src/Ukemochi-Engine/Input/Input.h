/* Start Header
*****************************************************************/
/*!
\file       Input.h
\author     Hurng Kai Rui, h.kairui, 2301278
\par        email: h.kairui\@digipen.edu
\date       Sept 23, 2024
\brief      This file declares the Input class, providing static methods for 
            handling keyboard and mouse input within the Ukemochi Engine.


Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include "KeyCode.h"
#include "MouseButtonCode.h"
#include <PreCompile.h>

namespace UME {
	/*!
	\class Input
	\brief A utility class for handling input operations such as key presses and mouse actions.
	*/
	class Input
	{
    public:
        /*!
        \brief Checks if a key is currently pressed.
        \param Keycode The key code of the key to check.
        \return True if the key is pressed, false otherwise.
        */
        static bool IsKeyPressed(int Keycode);

        /*!
        \brief Checks if a key is triggered (pressed for the first time).
        \param Keycode The key code of the key to check.
        \return True if the key is triggered, false otherwise.
        */
        static bool IsKeyTriggered(int Keycode);

        /*!
        \brief Checks if a mouse button is currently pressed.
        \param Mousecode The mouse button code to check.
        \return True if the mouse button is pressed, false otherwise.
        */
        static bool IsMouseButtonPressed(int Mousecode);

        /*!
        \brief Checks if a mouse button is triggered (pressed for the first time).
        \param Mousecode The mouse button code to check.
        \return True if the mouse button is triggered, false otherwise.
        */
        static bool IsMouseButtonTriggered(int Keycode);

        /*!
        \brief Gets the current mouse position.
        \return A pair containing the x and y coordinates of the mouse.
        */
        static std::pair<float, float> GetMousePosition();

        /*!
        \brief Gets the current x coordinate of the mouse position.
        \return The x coordinate of the mouse.
        */
        static float GetMouseX();

        /*!
        \brief Gets the current y coordinate of the mouse position.
        \return The y coordinate of the mouse.
        */
        static float GetMouseY();
    };

}