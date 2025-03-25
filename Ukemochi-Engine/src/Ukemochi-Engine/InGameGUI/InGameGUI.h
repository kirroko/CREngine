/* Start Header ************************************************************************/
/*!
\file       InGameGUI.h
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\co-author	TAN Shun Zhi Tomy, t.shunzhitomy, 2301341, t.shunzhitomy\@digipen.edu
\date       Feb 6, 2025
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
#include "../Factory/GameObject.h"
#include "../Game/GSM.h"
#include <unordered_map> // Include this for std::unordered_map

namespace Ukemochi
{
	class InGameGUI : public System
	{
	private:
		EntityID id = (EntityID)(-1);
		bool show_fps = false; // Show FPS debug text
		bool show_stats = false; // Show stats text
		std::unordered_map<std::string, bool> buttonHoverState; // Tracks whether a button was hovered last frame
		float hoverCooldown = 0.5f;  // cooldown for hover sound
		float hoverTimer = 0.0f;     // Tracks time since last hover sound

		float scrollSpeed = 250.f; // Pixels per second (adjust speed)

		float deathTimer = 0.f;
		const float deathScreenDelay = 2.f;
	public:
		bool showCredits = false; // Show credits
		
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
		void CreateText(const std::string& id, const std::string& label, const Vec2& pos, const float scale, const Vec3& color, const std::string& font_name);

		/*!***********************************************************************
		\brief
		 Update the label of an existing GUI text object.
		\param[in] id
		 The ID of the text object to update.
		\param[in] new_label
		 The new text to set as the label.
		*************************************************************************/
		void UpdateText(const std::string& id, const std::string& new_label);

		/*!***********************************************************************
		\brief
		 Creates various GUI images (e.g., health bar, icons, etc.).
		*************************************************************************/
		void CreateImage();

		/*!***********************************************************************
		\brief
		 Removes various GUI images (e.g., health bar, icons, etc.).
		*************************************************************************/
		void RemoveGameUI();

		/*!***********************************************************************
		\brief
		 Creates various main menu UI (e.g., start, credits, etc.).
		*************************************************************************/
		void CreateMainMenuUI();

		/*!***********************************************************************
		\brief
		 Creates credit screen.
		*************************************************************************/
		void ShowCredits();

		/*!***********************************************************************
		\brief
		 Updates credit screen to be able to scroll.
		*************************************************************************/
		void UpdateCredits();

		/*!***********************************************************************
		\brief
		 Updates ukemochi title to have the drop down effect.
		*************************************************************************/
		void UpdateTitleAnimation();

		/*!***********************************************************************
		\brief
		 Remove a GUI object.
		\param[in] id
		 The ID for the GUI object.
		*************************************************************************/
		void RemoveElement(const std::string& id);

		/*!***********************************************************************
		\brief
		 Remove a text object.
		\param[in] id
		 The ID for the text object.
		*************************************************************************/
		void RemoveText(const std::string& id);

		/*!***********************************************************************
		\brief
		 Renders the GUI elements based on the current camera position.
		\param[in] cameraPos
		 The current position of the camera.
		*************************************************************************/
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

		/*!***********************************************************************
		\brief
		 Displays the in-game pause menu.
		*************************************************************************/
		void ShowPauseMenu();

		/*!***********************************************************************
		\brief
		 Hides the in-game pause menu.
		*************************************************************************/
		void HidePauseMenu();

		/*!***********************************************************************
		\brief
		 Displays the defeat screen when the player loses.
		*************************************************************************/
		void showDefeatScreen();

		/*!***********************************************************************
		\brief
		 Hides the defeat screen and resumes gameplay.
		*************************************************************************/
		void HideDefeatScreen();

		/*!***********************************************************************
		\brief
		 Displays the stats menu.
		*************************************************************************/
		void ShowStats();

		/*!***********************************************************************
		\brief
		 Hides the stats menu and resumes gameplay.
		*************************************************************************/
		void HideStats();

		/*!***********************************************************************
		\brief
		 Updates the music volume bar UI based on the current audio volume level.
		*************************************************************************/
		void UpdateMusicBar();

		/*!***********************************************************************
		\brief
		 Updates the SFX volume bar UI based on the current audio volume level.
		*************************************************************************/
		void UpdateSFXBar();
	};
}