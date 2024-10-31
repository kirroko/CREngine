/* Start Header
*****************************************************************/
/*!
\file       InGameGUI.h
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Oct 31, 2024
\brief      This file contains the declaration of the in game GUI system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#include <vector> // for std::vector
#include "GUIElement.h" // for GUI element

namespace Ukemochi
{
	class InGameGUI
	{
	private:
		std::vector<GUIElement*> elements;

	public:
		void CreateButton(const Vec2& pos, const Vec2& scale, const std::string& text, std::function<void()> on_click);
		void CreateTextBox(const Vec2& pos, const Vec2& scale, const std::string& text);

		void Update();

		void HandleMouseClick();
		void HandleTextInput(char input);

		void Render();

		~InGameGUI();

	private:
		void AddElement(GUIElement* element);
	};
}