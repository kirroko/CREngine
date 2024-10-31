/* Start Header
*****************************************************************/
/*!
\file       GUIElement.cpp
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Oct 31, 2024
\brief      This file contains the definition of the GUI element class.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "PreCompile.h"
#include "GUIElement.h" // for forward declaration
#include "../Input/Input.h" // for mouse position

namespace Ukemochi
{
	GUIElement::GUIElement(const Vec2& pos, const Vec2& scale, const std::string& text)
		: transform{ Mtx44{}, pos, 0.0f, scale }, text(text), enabled(true) {}

	bool GUIElement::IsInside()
	{
		auto [x, y] = UME::Input::GetMousePosition();
		return x >= transform.position.x
			&& y >= transform.position.y
			&& x <= transform.position.x + transform.scale.x
			&& y <= transform.position.y + transform.scale.y;
	}
}