/* Start Header ************************************************************************/
/*!
\file       InGameGUI.h
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Nov 2, 2024
\brief      This file contains the declaration of the in game GUI system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include "../ECS/ECS.h" // for ECS system and components
#include "../Factory/GameObject.h"

namespace Ukemochi
{
	class InGameGUI : public System
	{
	public:
		/*!***********************************************************************
		\brief
		 Initialize the in game GUI system.
		*************************************************************************/
		void Init();

		/*!***********************************************************************
		\brief
		 Update the inputs of the in game GUI system.
		*************************************************************************/
		void Update();

		/*!***********************************************************************
		\brief
		 Create a GUI text object.
		*************************************************************************/
		void CreateText(const std::string& id, const std::string& label, const Vec2& pos, const float scale, const Vec3& color, const std::string& font_name);

		/*!***********************************************************************
		\brief
		 Update a GUI text object label value.
		*************************************************************************/
		void UpdateText(const std::string& id, const std::string& new_label);

		/*!***********************************************************************
		\brief
		 Create a GUI image object.
		*************************************************************************/
		void CreateImage(const Vec2& pos, const Vec2& scale, const std::string& texture_path);

		/*!***********************************************************************
		\brief
		 Create a GUI button object.
		*************************************************************************/
		void CreateButton(const std::string& id, const std::string& label, const Vec2& pos, const float label_scale, const Vec3& color, const std::string& font_name, const Vec2& button_scale, const std::string& texture_path, std::function<void()> on_click);

		void CreateButtonOBJ(const std::string& btn, const std::string& btntag, const std::string& id, const std::string& label, const Vec2& pos, const float label_scale, const Vec3& color, const std::string& font_name, const Vec2& button_scale, const std::string& texture_path, std::function<void()> on_click);

	private:
		/*!***********************************************************************
		\brief
		 Handle the GUI button inputs.
		*************************************************************************/
		void HandleButtonInput();

		/*!***********************************************************************
		\brief
		 Check if the mouse is within the GUI object boundaries.
		*************************************************************************/
		bool IsInside(const Transform& trans);
	};
}