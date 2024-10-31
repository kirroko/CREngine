/* Start Header
*****************************************************************/
/*!
\file       TextBox.h
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Oct 31, 2024
\brief      This file contains the declaration of the GUI text box class.

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
	class TextBox : public GUIElement
	{
	private:
		std::string text;

	public:
		TextBox(const Vec2& pos, const Vec2& scale, const std::string& text);

		void HandleClick() override;
		void HandleTextInput(char input) override;
		void Render() override;
	};
}