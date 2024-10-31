/* Start Header
*****************************************************************/
/*!
\file       TextBox.cpp
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Oct 31, 2024
\brief      This file contains the definition of the GUI text box class.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "PreCompile.h"
#include "TextBox.h" // for forward declaration

namespace Ukemochi
{
	TextBox::TextBox(const Vec2& pos, const Vec2& scale, const std::string& text)
		: GUIElement(pos, scale, text), text(text) {}

	void TextBox::HandleClick()
	{
		if (IsInside())
			std::cout << "clicked";
	}

	void TextBox::HandleTextInput(char input)
	{
		text += input;
	}

	void TextBox::Render()
	{

	}
}