/* Start Header
*****************************************************************/
/*!
\file       InGameGUI.cpp
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Nov 2, 2024
\brief      This file contains the definition of the in game GUI system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "PreCompile.h"
#include "InGameGUI.h"				// for forward declaration
#include "../Input/Input.h"			// for mouse and key inputs
#include "../Application.h"			// for screen size
#include "../Factory/Factory.h"		// for game objects
#include "../Graphics/Camera2D.h"	// for camera viewport
#include "../Graphics/Renderer.h"	// for text objects

namespace Ukemochi
{
	/*!***********************************************************************
	\brief
	 Initialize the in game GUI system.
	*************************************************************************/
	void InGameGUI::Init()
	{
		// Get the screen width and height
		Application& app = Application::Get();
		int screen_width = app.GetWindow().GetWidth();
		int screen_height = app.GetWindow().GetHeight();

		// Create some test GUI elements
		CreateImage(Vec2{ screen_width * 0.05f, screen_height * 0.9f }, Vec2{ 75.f, 150.f }, "../Assets/Textures/UI/game_logo.png");

		CreateText("text1", "pls click a button", Vec2{ screen_width * 0.1f, screen_height * 0.9f }, 1.f, Vec3{ 1.f, 1.f, 1.f }, "Ukemochi");

		CreateButton("pause_btn", "", Vec2{ screen_width * 0.05f, screen_height * 0.8f }, 1.f, Vec3{ 1.f, 1.f, 1.f }, "Ukemochi",
			Vec2{ 75.f, 75.f }, "../Assets/Textures/UI/pause.png", [this]() { UpdateText("text1", "pause button clicked!"); });

		CreateButton("spawn_btn", "Spawn", Vec2{ screen_width * 0.15f, screen_height * 0.8f }, 1.f, Vec3{ 1.f, 0.f, 0.f }, "Ukemochi",
			Vec2{ 150.f, 75.f }, "../Assets/Textures/UI/base.png", [this]()
			{
				UpdateText("text1", "spawn button clicked!");

				// Spawn a enemy
				GameObject enemy = GameObjectFactory::CreateObject();
				enemy.AddComponent(Transform{
					Mtx44{},
					Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.75f,
					ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.75f},
					0,
					Vec2{100.f, 100.f}
					});
				enemy.AddComponent(Rigidbody2D{ Vec2{}, Vec2{750.f, 750.f}, Vec2{}, Vec2{},1.f, 1.f, 0.9f, 0.f,0.f,0.f,0.f,1.f, 1.f, 0.9f, false, false });
				enemy.AddComponent(BoxCollider2D());
				enemy.GetComponent<BoxCollider2D>().tag = "Enemy";
				enemy.AddComponent(SpriteRender{ "../Assets/Textures/Worm.png", SPRITE_SHAPE::BOX, });
			});
	}

	/*!***********************************************************************
	\brief
	 Update the inputs of the in game GUI system.
	*************************************************************************/
	void InGameGUI::Update()
	{
		// Handle button inputs
		HandleButtonInput();

		/*for (auto const& entity : m_Entities)
		{
			auto& trans = ECS::GetInstance().GetComponent<Transform>(entity);
			auto& button = ECS::GetInstance().GetComponent<Button>(entity);
			auto& camera = ECS::GetInstance().GetSystem<Camera>();

			trans.position = Vec2{ button.initial_pos.x + camera->position.x, button.initial_pos.y + camera->position.y };
		}*/
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
	void InGameGUI::CreateImage(const Vec2& pos, const Vec2& scale, const std::string& texture_path)
	{
		GameObject image = GameObjectFactory::CreateObject();
		image.AddComponent(Transform{ Mtx44{}, pos, 0, scale });
		image.AddComponent(SpriteRender{ texture_path });
	}

	/*!***********************************************************************
	\brief
	 Create a GUI button object.
	*************************************************************************/
	void InGameGUI::CreateButton(const std::string& id, const std::string& label, const Vec2& pos, const float label_scale, const Vec3& color, const std::string& font_name, const Vec2& button_scale, const std::string& texture_path, std::function<void()> on_click)
	{
		GameObject button = GameObjectFactory::CreateObject();
		button.AddComponent(Transform{ Mtx44{}, pos, 0, button_scale });
		button.AddComponent(SpriteRender{ texture_path });
		button.AddComponent(Button{ on_click });

		// Offset the text position to make it left and middle aligned
		Vec2 text_pos = Vec2{ pos.x - button_scale.x * 0.4f, pos.y - button_scale.y * 0.25f };
		ECS::GetInstance().GetSystem<Renderer>()->CreateTextObject(id, label, text_pos, label_scale, color, font_name);
	}

	/*!***********************************************************************
	\brief
	 Handle the GUI button inputs.
	*************************************************************************/
	void InGameGUI::HandleButtonInput()
	{
		// Check for mouse left click
		if (Input::IsMouseButtonTriggered(UME_MOUSE_BUTTON_1))
		{
			for (auto const& entity : m_Entities)
			{
				auto& trans = ECS::GetInstance().GetComponent<Transform>(entity);
				auto& button = ECS::GetInstance().GetComponent<Button>(entity);

				// Skip if the button is not interactable
				if (!button.interactable)
					continue;

				// Check if the mouse is within the button boundaries
				if (IsInside(trans))
					button.on_click();
			}
		}
	}

	/*!***********************************************************************
	\brief
	 Check if the mouse is within the GUI object boundaries.
	*************************************************************************/
	bool InGameGUI::IsInside(const Transform& trans)
	{
		// Get current mouse position in screen coordinates
		auto [mouse_x, mouse_y] = Input::GetMousePosition();

		// Flip the mouse position in the y-axis
		mouse_y = ECS::GetInstance().GetSystem<Camera>()->viewport_size.y - mouse_y;

		// Offset mouse position
		mouse_x += trans.scale.x * 0.5f;
		mouse_y += trans.scale.y * 0.5f;

		return mouse_x >= trans.position.x
			&& mouse_y >= trans.position.y
			&& mouse_x <= trans.position.x + trans.scale.x
			&& mouse_y <= trans.position.y + trans.scale.y;
	}
}