/* Start Header
*****************************************************************/
/*!
\file       Button.cpp
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Oct 31, 2024
\brief      This file contains the definition of the GUI button class.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "PreCompile.h"
#include "Button.h" // for forward declaration

namespace Ukemochi
{
	Button::Button(const Vec2& pos, const Vec2& scale, const std::string& text, std::function<void()> on_click)
		: GUIElement(pos, scale, text), on_click(on_click) {}

	void Button::HandleClick()
	{
		if (IsInside())
			on_click();
	}

	void Button::Render()
	{

	}
}