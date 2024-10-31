/* Start Header
*****************************************************************/
/*!
\file       GUIElement.h
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Oct 31, 2024
\brief      This file contains the declaration of the GUI element class.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#include "../ECS/Components.h" // for Transform component
#include <string> // for std::string

namespace Ukemochi
{
	class GUIElement
	{
	private:
		Transform transform;
		std::string text;
		bool enabled;

	public:
		GUIElement(const Vec2& pos, const Vec2& scale, const std::string& text);
		virtual ~GUIElement() = default;

		virtual void HandleClick() = 0;
		virtual void HandleTextInput(char input) {}
		virtual void Render() = 0;

		bool IsInside();
	};
}