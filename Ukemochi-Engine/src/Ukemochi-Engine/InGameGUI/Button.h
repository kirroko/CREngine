/* Start Header
*****************************************************************/
/*!
\file       Button.h
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Oct 31, 2024
\brief      This file contains the declaration of the GUI button class.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#include "GUIElement.h"

namespace Ukemochi
{
    class Button : public GUIElement
    {
	private:
		std::function<void()> on_click;

	public:
		Button(const Vec2& pos, const Vec2& scale, const std::string& text, std::function<void()> on_click);

		void HandleClick() override;
		void Render() override;
    };
}