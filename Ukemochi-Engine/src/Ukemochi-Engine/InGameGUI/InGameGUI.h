/* Start Header ************************************************************************/
/*!
\file       InGameGUI.h
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Nov 27, 2024
\brief      This file contains the declaration of the in game GUI system which handles GUI
			elements such as text, images and buttons within the game.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include "../ECS/ECS.h"			  // for ECS system and components
#include "../Graphics/UIButton.h" // for TextAlignment

namespace Ukemochi
{
	class InGameGUI : public System
	{
	private:
		bool show_fps = false; // Show FPS debug text
		 
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
		\param[in] id
		 The ID for the text object.
		\param[in] label
		 The text to be displayed.
		\param[in] pos
		 The position of the text.
		\param[in] color
		 The color of the text (RGB format).
		\param[in] font_name
		 The font to be used for rendering the text.
		*************************************************************************/
		//void CreateText(const std::string& id, const std::string& label, const Vec2& pos, const float scale, const Vec3& color, const std::string& font_name);

		/*!***********************************************************************
		\brief
		 Update the label of an existing GUI text object.
		\param[in] id
		 The ID of the text object to update.
		\param[in] new_label
		 The new text to set as the label.
		*************************************************************************/
		//void UpdateText(const std::string& id, const std::string& new_label);

		/*!***********************************************************************
		\brief
		 Create a GUI image object.
		\param[in] id
		 The ID for the image object.
		\param[in] pos
		 The position of the image.
		\param[in] size
		 The size of the image.
		\param[in] textureID
		 The ID for the image texture.
		*************************************************************************/
		void CreateImage();

		/*!***********************************************************************
		\brief
		 Create a GUI button object.
		\param[in] id
		 The ID for the button object.
		\param[in] pos
		 The position of the button.
		\param[in] size
		 The size of the button.
		\param[in] textureID
		 The ID for the button texture.
		\param[in] text
		 The label text displayed on the button.
		\param[in] textColor
		 The color of the label text (RGB format).
		\param[in] fontName
		 The font to be used for the label text.
		\param[in] textScale
		 The scale of the label text.
		\param[in] alignment
		 The alignment of the text within the button.
		\param[in] interactable
		 Whether the button can be interacted with.
		\param[in] on_click
		 A callback function triggered on button click.
		*************************************************************************/
		void CreateButton();

		/*!***********************************************************************
		\brief
		 Remove a GUI object.
		\param[in] id
		 The ID for the GUI object.
		*************************************************************************/
		void RemoveElement(const std::string& id);

		void Render(glm::vec3& cameraPos);

	private:
		/*!***********************************************************************
		\brief
		 Handle the GUI button inputs.
		*************************************************************************/
		void HandleButtonInput();

		/*!***********************************************************************
		\brief
		 Check if the mouse is within the GUI object boundaries.
		\param[in] pos
		 The position of the GUI object.
		\param[in] size
		 The size of the GUI object.
		\return
		 True if the mouse is within the object's boundaries, false otherwise.
		*************************************************************************/
		bool IsInside(const Vec2& pos, const Vec2& size);

		void ShowPauseMenu();
		void HidePauseMenu();
	};
}