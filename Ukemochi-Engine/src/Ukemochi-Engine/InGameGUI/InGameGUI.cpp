/* Start Header
*****************************************************************/
/*!
\file       InGameGUI.cpp
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Oct 31, 2024
\brief      This file contains the definition of the in game GUI system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "PreCompile.h"
#include "InGameGUI.h"		// for forward declaration
#include "Button.h"			// for Button class
#include "TextBox.h"		// for TextBox class
#include "../Input/Input.h" // for mouse and key inputs

namespace Ukemochi
{
	void InGameGUI::CreateButton(const Vec2& pos, const Vec2& scale, const std::string& text, std::function<void()> on_click)
	{
		Button* button = new Button(pos, scale, text, on_click);
		AddElement(button);
		std::cout << "New button created.\n";
	}

	void InGameGUI::CreateTextBox(const Vec2& pos, const Vec2& scale, const std::string& text)
	{
		TextBox* text_box = new TextBox(pos, scale, text);
		AddElement(text_box);
		std::cout << "New textbox created.\n";
	}

	void InGameGUI::Update()
	{
		if (UME::Input::IsMouseButtonPressed(UME_MOUSE_BUTTON_1))
			HandleMouseClick();

		//if(UME::Input::IsKeyTriggered() // Check for key presses and pass into text box
			HandleTextInput('T');
	}

	void InGameGUI::HandleMouseClick()
	{
		for (auto element : elements)
			element->HandleClick();
	}

	void InGameGUI::HandleTextInput(char input)
	{
		for (auto element : elements)
			element->HandleTextInput(input);
	}

	void InGameGUI::Render()
	{
		for (auto element : elements)
			element->Render();
	}

	InGameGUI::~InGameGUI()
	{
		for (auto element : elements)
			delete element;
	}

	void InGameGUI::AddElement(GUIElement* element) { elements.push_back(element); }
}