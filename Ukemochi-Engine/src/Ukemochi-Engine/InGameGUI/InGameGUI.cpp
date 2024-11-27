/* Start Header ************************************************************************/
/*!
\file       InGameGUI.cpp
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Nov 2, 2024
\brief      This file contains the definition of the in game GUI system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "InGameGUI.h"				// for forward declaration
#include "../Input/Input.h"			// for mouse and key inputs
#include "../Application.h"			// for screen size
#include "../Factory/Factory.h"		// for game objects
#include "../Graphics/Camera2D.h"	// for camera viewport
#include "../Graphics/Renderer.h"	// for text objects
#include "Ukemochi-Engine/Factory/GameObjectManager.h"
#include "../SceneManager.h"

namespace Ukemochi
{
	/*!***********************************************************************
	\brief
	 Initialize the in game GUI system.
	*************************************************************************/
	void InGameGUI::Init()
	{
		//Create some test GUI elements
		CreateImage("gameUI", Vec2{ 960.f, 540.f }, Vec2{ 1920.f, 1080.f }, 9);

		//CreateButton("pauseButton", Vec2{ 100.f, 700.f }, Vec2{ 125.f, 75.f }, 12, "Pause", Vec3{ 1.f, 0.f, 0.f }, "Exo2", 0.75f, TextAlignment::Center, true,
		//	[]() {
		//		std::cout << "PAUSE PRESSED\n";
		//		ECS::GetInstance().GetSystem<InGameGUI>()->UpdateText("text1", "pause button clicked!");
		//	});
	}

	/*!***********************************************************************
	\brief
	 Update the inputs of the in game GUI system.
	*************************************************************************/
	void InGameGUI::Update()
	{
		// Handle button inputs
		HandleButtonInput();
	}

	/*!***********************************************************************
	\brief
	 Create a GUI text object.
	*************************************************************************/
	void InGameGUI::CreateText(const std::string& id, const std::string& label, const Vec2& pos, const float scale, const Vec3& color, const std::string& font_name)
	{
		ECS::GetInstance().GetSystem<Renderer>()->CreateTextObject(id, label, pos, scale, color, font_name);
	}

	/*!***********************************************************************
	\brief
	 Update a GUI text object label value.
	*************************************************************************/
	void InGameGUI::UpdateText(const std::string& id, const std::string& new_label)
	{
		ECS::GetInstance().GetSystem<Renderer>()->UpdateTextObject(id, new_label);
	}

	/*!***********************************************************************
	\brief
	 Create a GUI image object.
	*************************************************************************/
	void InGameGUI::CreateImage(const std::string& id, const Vec2& position, const Vec2& size, int textureID)
	{
		ECS::GetInstance().GetSystem<Renderer>()->CreateButtonObject(id, position, size, textureID, "", Vec3{ 0.f, 0.f, 0.f }, "Exo2", 1.f, TextAlignment::Center, false, nullptr);
	}

	/*!***********************************************************************
	\brief
	 Create a GUI button object.
	*************************************************************************/
	void InGameGUI::CreateButton(const std::string& id, const Vec2& position, const Vec2& size, int textureID, const std::string& text, const Vec3& textColor, std::string fontName, float textScale, TextAlignment alignment, bool interactable, std::function<void()> on_click)
	{
		ECS::GetInstance().GetSystem<Renderer>()->CreateButtonObject(id, position, size, textureID, text, textColor, fontName, textScale, alignment, interactable, on_click);
	}

	//void InGameGUI::CreateButtonOBJ(const std::string& btn, const std::string& btntag, const std::string& id, const std::string& label, const Vec2& pos, const float label_scale, const Vec3& color, const std::string& font_name, const Vec2& button_scale, const std::string& texture_path, std::function<void()> on_click)
	//{
	//	//GameObject button = GameObjectManager::GetInstance().CreateObject(btn,btntag);
	//	//button.AddComponent(Transform{ Mtx44{}, pos, 0, button_scale });
	//	//button.AddComponent(SpriteRender{ texture_path });
	//	//button.AddComponent(Button{ on_click });

	//	//// Offset the text position to make it left and middle aligned
	//	//Vec2 text_pos = Vec2{ pos.x - button_scale.x * 0.4f, pos.y - button_scale.y * 0.25f };
	//	//ECS::GetInstance().GetSystem<Renderer>()->CreateTextObject(id, label, text_pos, label_scale, color, font_name);
	//}

	/*!***********************************************************************
	\brief
	 Handle the GUI button inputs.
	*************************************************************************/
	void InGameGUI::HandleButtonInput()
	{
		// Check for mouse left click
		if (Input::IsMouseButtonTriggered(UME_MOUSE_BUTTON_1))
		{
			for (auto const& button : ECS::GetInstance().GetSystem<Renderer>()->GetButtonObjects())
			{
				// Skip if the button is not interactable
				if (!button.interactable)
					continue;

				// Check if the mouse is within the button boundaries
				if (IsInside(Vec2{ button.position.x, button.position.y }, Vec2{ button.size.x, button.size.y }))
					button.on_click(); // Invoke the mouse on click event
			}
		}
	}

	/*!***********************************************************************
	\brief
	 Check if the mouse is within the GUI object boundaries.
	*************************************************************************/
	bool InGameGUI::IsInside(const Vec2& position, const Vec2& size)
	{
		// Get current mouse position in screen coordinates
		float mouse_x = SceneManager::GetInstance().GetPlayScreen().x + ECS::GetInstance().GetSystem<Camera>()->position.x;
		float mouse_y = SceneManager::GetInstance().GetPlayScreen().y + ECS::GetInstance().GetSystem<Camera>()->position.y;
		//auto [mouse_x, mouse_y] = SceneManager::GetInstance().GetPlayScreen();//Input::GetMousePosition();

		// Flip the mouse position in the y-axis
		//mouse_y = ECS::GetInstance().GetSystem<Camera>()->viewport_size.y - mouse_y;

		//std::cout << "button" << " : " << position.x << ", " << position.y << std::endl;
		std::cout << "mouse" << " : " << mouse_x << ", " << mouse_y << std::endl;

		return mouse_x >= position.x - size.x * 0.5f
			&& mouse_x <= position.x + size.x
			&& mouse_y >= position.y - size.y * 0.5f + 300.f
			&& mouse_y <= position.y + size.y + 300.f;

		/*return mouse_y <= position.y - 20.f
			&& mouse_y >= position.y - size.y - 20.f
			&& mouse_x >= position.x - size.x
			&& mouse_x <= position.x + size.x;*/
	}
}
