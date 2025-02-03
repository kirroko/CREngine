/* Start Header ************************************************************************/
/*!
\file       InGameGUI.cpp
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Nov 27, 2024
\brief      This file contains the definition of the in game GUI system which handles GUI
			elements such as text, images and buttons within the game.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "InGameGUI.h"			  // for forward declaration
#include "../Input/Input.h"		  // for mouse and key inputs
#include "../Application.h"		  // for screen size
#include "../Graphics/Camera2D.h" // for camera viewport
#include "../Graphics/Renderer.h" // for text objects
#include "../FrameController.h"	  // for fps text

namespace Ukemochi
{
	/*!***********************************************************************
	\brief
	 Initialize the in game GUI system.
	*************************************************************************/
	void InGameGUI::Init()
	{
#ifndef _DEBUG
		//// Get the screen width and height
		//Application& app = Application::Get();
		//int screen_width = app.GetWindow().GetWidth();
		//int screen_height = app.GetWindow().GetHeight();

		////Create the game UI, elements are combined temporary
		//// Create game UI screen
		////CreateImage("game", Vec2{ screen_width * 0.5f, screen_height * 0.5f }, Vec2{ static_cast<float>(screen_width), static_cast<float>(screen_height) }, 28);

		////// Create main menu screen
		////CreateImage("main_menu", Vec2{ screen_width * 0.5f, screen_height * 0.5f }, Vec2{ static_cast<float>(screen_width), static_cast<float>(screen_height) }, 29);

		////// Create start menu button
		////CreateButton("start_button", Vec2{ 1168.f, 478.f }, Vec2{ 464.f, 243.f }, 27, "", Vec3{ 1.f, 1.f, 1.f }, "Ukemochi", 1.f, TextAlignment::Center, true,
		////	[]() { Application::Get().StartGame(); });

		//// Create FPS text
		////CreateText("fps_text", "", Vec2{ screen_width * 0.01f, screen_height * 0.95f }, 1.5f, Vec3{ 1.f, 1.f, 1.f }, "Ukemochi_numbers");

		//CreateImage(Vec3{ screen_width * 0.5f, screen_height * 0.5f , 0.f }, Vec2{ static_cast<float>(screen_width), static_cast<float>(screen_height) }, "ui_mainmenu", 0, Vec3(1.f, 1.f, 1.f));

		CreateImage();

#endif // !_DEBUG

		//CreateButton("pauseButton", Vec2{ 1840.f, 1010.f }, Vec2{ 75.f, 75.f }, 10, "P", Vec3{ 1.f, 1.f, 1.f }, "Ukemochi", 1.f, TextAlignment::Center, true,
		//	[this]() { UpdateText("text1", "pause clicked!"); });

		//CreateButton("abilityButton", Vec2{ 1700.f, 75.f }, Vec2{ 100.f, 100.f }, 10, "F", Vec3{ 1.f, 1.f, 1.f }, "Ukemochi", 1.f, TextAlignment::Center, true,
		//	[this]() { UpdateText("text1", "ability clicked!"); });

		//CreateButton("swapButton", Vec2{ 1840.f, 75.f }, Vec2{ 100.f, 100.f }, 10, "Q", Vec3{ 1.f, 1.f, 1.f }, "Ukemochi", 1.f, TextAlignment::Center, true,
		//	[this]() { UpdateText("text1", "swap clicked!"); });

		//CreateImage();
	}

	/*!***********************************************************************
	\brief
	 Update the inputs of the in game GUI system.
	*************************************************************************/
	void InGameGUI::Update()
	{
		// Handle button inputs
		HandleButtonInput();

		// Update fps text
		/*if(show_fps)
			UpdateText("fps_text", std::to_string(static_cast<int>(g_FrameRateController.GetFPS())));
		else
			UpdateText("fps_text", "");*/
	}

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
	void InGameGUI::CreateText(const std::string& id, const std::string& label, const Vec2& pos, const float scale, const Vec3& color, const std::string& font_name)
	{
		//ECS::GetInstance().GetSystem<Renderer>()->CreateTextObject(id, label, pos, scale, color, font_name);
	}

	/*!***********************************************************************
	\brief
	 Update the label of an existing GUI text object.
	\param[in] id
	 The ID of the text object to update.
	\param[in] new_label
	 The new text to set as the label.
	*************************************************************************/
	void InGameGUI::UpdateText(const std::string& id, const std::string& new_label)
	{
		//ECS::GetInstance().GetSystem<Renderer>()->UpdateTextObject(id, new_label);
	}

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
	/*void InGameGUI::CreateImage(const std::string& id, const Vec3& pos, const Vec2& size, const std::string& spriteName, int layer, const Vec3& color, BarType barType)
	{
		ECS::GetInstance().GetSystem<Renderer>()->CreateButtonObject(id, pos, size, spriteName, Vec3{ 0.f, 0.f, 0.f }, layer, barType, nullptr);
	}*/

	/*!***********************************************************************
	\brief
	 Create a GUI button object.
	*************************************************************************/
	void InGameGUI::CreateImage()
	{
		auto& uiManager = ECS::GetInstance().GetSystem<UIButtonManager>();
		Application& app = Application::Get(); 
		int screen_width = app.GetWindow().GetWidth(); 
		int screen_height = app.GetWindow().GetHeight(); 

		//--Health---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		uiManager->addButton("health bar bg", glm::vec3(353.f, 1000.f, 0.f), glm::vec2(627.f, 66.f), "in game_health bar bg", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, []() {
			});
		uiManager->addButton("health bar", glm::vec3(353.f, 1000.f, 0.f), glm::vec2(627.f, 66.f), "in game_health bar", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::Health, []() {
			});
		uiManager->addButton("health bar border", glm::vec3(356.f, 1003.f, 0.f), glm::vec2(598.f, 36.f), "in game_health bar border", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, []() {
			});
		uiManager->addButton("health bar icon", glm::vec3(53.f, 1000.f, 0.f), glm::vec2(117.f, 129.f), "in game_health bar icon", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, []() {
			});
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		//--Soul blue----------------------------------------------------------------------------------------------------------------------------------------------------------------------
		uiManager->addButton("blue soul bar bg", glm::vec3(300.f, 941.f, 0.f), glm::vec2(403.f, 35.f), "in game_soul bar bg", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, []() {
			});

		uiManager->addButton("blue soul bar", glm::vec3(300.f, 941.5f, 0.f), glm::vec2(357.f, 17.f), "in game_soul bar blue", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::Blue_Soul, []() {
			});

		uiManager->addButton("blue soul bar border", glm::vec3(300.f, 943.f, 0.f), glm::vec2(357.f, 18.f), "in game_soul bar border", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, []() {
			});

		uiManager->addButton("blue fire", glm::vec3(130.f, 943.f, 0.f), glm::vec2(65.f, 71.f), "in game_blue fire", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 2, BarType::None, []() {
			});

		uiManager->addButton("blue soul ability charge bg", glm::vec3(500.f, 941.5f, 0.f), glm::vec2(58.f, 61.f), "in game_soul ability charge bg", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, []() {
			});

		uiManager->addButton("blue soul ability charge bar", glm::vec3(500.f, 941.f, 0.f), glm::vec2(37.f, 17.f), "in game_soul bar red", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::Blue_Charge_Bar, []() {
			});

		uiManager->addButton("blue soul ability charge border", glm::vec3(500.5f, 944.5f, 0.f), glm::vec2(25.f, 30.f), "in game_soul ability charge border", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, []() {
			});
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		//--Soul red----------------------------------------------------------------------------------------------------------------------------------------------------------------------
		uiManager->addButton("red soul bar bg", glm::vec3(333.f, 881.f, 0.f), glm::vec2(403.f, 35.f), "in game_soul bar bg", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, []() {
			});

		uiManager->addButton("red soul bar", glm::vec3(333.f, 881.f, 0.f), glm::vec2(357.f, 17.f), "in game_soul bar red", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::Red_Soul, []() {
			});

		uiManager->addButton("red soul bar border", glm::vec3(333.f, 883.f, 0.f), glm::vec2(357.f, 18.f), "in game_soul bar border", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, []() {
			});

		uiManager->addButton("red fire", glm::vec3(163.f, 883.f, 0.f), glm::vec2(65.f, 71.f), "in game_red fire", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 2, BarType::None, []() {
			});

		uiManager->addButton("red soul ability charge bg", glm::vec3(533.f, 881.f, 0.f), glm::vec2(58.f, 61.f), "in game_soul ability charge bg", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, []() {
			});

		uiManager->addButton("red soul ability charge bar", glm::vec3(533.f, 881.f, 0.f), glm::vec2(37.f, 17.f), "in game_soul bar red", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::Red_Charge_Bar, []() {
			});

		uiManager->addButton("red soul ability charge border", glm::vec3(534.f, 884.75f, 0.f), glm::vec2(25.f, 30.f), "in game_soul ability charge border", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, []() {
			});
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		
		//--Ability and Pause----------------------------------------------------------------------------------------------------------------------------------------------------------------------
		uiManager->addButton("soul change", glm::vec3(1825.f, 100.f, 0.f), glm::vec2(119.f, 121.f), "in game_soul change", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, []() {
			});

		uiManager->addButton("game ability", glm::vec3(1675.f, 100.f, 0.f), glm::vec2(119.f, 121.f), "in game_abilities", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, []() {
			});

		uiManager->addButton("healthBar", glm::vec3(1825.f, 1000.f, 0.f), glm::vec2(119.f, 121.f), "in game_pause", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, []() {
			});
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		// Main Menu
		uiManager->addButton("main menu", glm::vec3{ screen_width * 0.5f, screen_height * 0.5f , 0.f }, glm::vec2{ static_cast<float>(screen_width), static_cast<float>(screen_height) }, "ui_mainmenu", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 2, BarType::None, []() {
			});

		uiManager->addButton("start button", glm::vec3{ 1168.f, 478.f, 0.f }, glm::vec2{ 464.f, 243.f }, "ui_button_start", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 3, BarType::None, []() {
			Application::Get().StartGame(); });
	}

	void InGameGUI::CreateButton()
	{

	}

	/*!***********************************************************************
	\brief
	 Remove a GUI object.
	\param[in] id
	 The ID for the GUI object.
	*************************************************************************/
	void InGameGUI::RemoveElement(const std::string& id)
	{
		ECS::GetInstance().GetSystem<UIButtonManager>()->removeButton(id);
	}

	void InGameGUI::Render(glm::vec3& cameraPos)
	{
		ECS::GetInstance().GetSystem<UIButtonManager>()->render(cameraPos);
	}

	/*!***********************************************************************
	\brief
	 Handle the GUI button inputs.
	*************************************************************************/
	void InGameGUI::HandleButtonInput()
	{
		//// Check for mouse left click
		//if (Input::IsMouseButtonPressed(UME_MOUSE_BUTTON_1))
		//{
		//	for (auto const& button : ECS::GetInstance().GetSystem<Renderer>()->GetButtonObjects())
		//	{
		//		// Skip if the button is not interactable
		//		if (!button.interactable)
		//			continue;

		//		// Check if the mouse is within the button boundaries
		//		if (IsInside(Vec2{ button.position.x, button.position.y }, Vec2{ button.size.x, button.size.y }))
		//			button.on_click(); // Invoke the button on click event
		//	}
		//}

		//// Press enter to start game
		//if (!Application::Get().GameStarted && Input::IsKeyTriggered(UME_KEY_ENTER))
		//{
		//	for (auto const& button : ECS::GetInstance().GetSystem<Renderer>()->GetButtonObjects())
		//	{
		//		// Skip if the button is not interactable
		//		if (!button.interactable)
		//			continue;

		//		// Check if it is the start button
		//		if(button.id == "start_button")
		//			button.on_click(); // Invoke the button on click event
		//	}
		//}

		//// Press F10 to toggle fps text
		//if (Input::IsKeyTriggered(UME_KEY_F10))
		//	show_fps = !show_fps;
	}

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
	bool InGameGUI::IsInside(const Vec2& pos, const Vec2& size)
	{
		// Get current mouse position
		auto [mouse_x, mouse_y] = Input::GetMousePosition();

		// Flip the mouse position in the y-axis
		mouse_y = ECS::GetInstance().GetSystem<Camera>()->viewport_size.y - mouse_y;

		return mouse_x >= pos.x - size.x * 0.5f
			&& mouse_x <= pos.x + size.x * 0.5f
			&& mouse_y >= pos.y - size.y * 0.5f
			&& mouse_y <= pos.y + size.y * 0.5f;
	}
}