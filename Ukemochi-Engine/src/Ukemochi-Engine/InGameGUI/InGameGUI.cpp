/* Start Header ************************************************************************/
/*!
\file       InGameGUI.cpp
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu (50%)
\co-author	TAN Shun Zhi Tomy, t.shunzhitomy, 2301341, t.shunzhitomy\@digipen.edu (40%)
\co-authors HURNG Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu (10%)
\date       Feb 6, 2025
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
#include "../Factory/GameObjectManager.h"
#include "../Video/VideoManager.h"

namespace Ukemochi
{
	bool startButtonHovered = false;
	/*!***********************************************************************
	\brief
	 Initialize the in game GUI system.
	*************************************************************************/
	void InGameGUI::Init()
	{
		Application& app = Application::Get();
		int screen_width = app.GetWindow().GetWidth();
		int screen_height = app.GetWindow().GetHeight();

		//CreateImage();
		CreateMainMenuUI();
		// Create FPS text
		CreateText("fps_text", "", Vec2{ screen_width * 0.92f, screen_height * 0.82f }, 1.5f, Vec3{1.f, 1.f, 1.f}, "Ukemochi_numbers");
	}

	/*!***********************************************************************
	\brief
	 Update the inputs of the in game GUI system.
	*************************************************************************/
	void InGameGUI::Update()
	{
		// Handle button inputs
		HandleButtonInput();

		auto& player = ECS::GetInstance().GetComponent<Player>(ECS::GetInstance().GetSystem<Renderer>()->getPlayerID());


		if (player.currentHealth <= 0)
		{
			ECS::GetInstance().GetSystem<InGameGUI>()->showDefeatScreen();
		}

		// Update FPS text with color based on FPS value
		if (show_fps)
		{
			int fps = static_cast<int>(g_FrameRateController.GetFPS());
			glm::vec3 fpsColor;

			if (fps > 60)
				fpsColor = glm::vec3(0.0f, 1.0f, 0.0f);  // Green
			else if (fps >= 30)
				fpsColor = glm::vec3(1.0f, 1.0f, 0.0f);  // Yellow
			else
				fpsColor = glm::vec3(1.0f, 0.0f, 0.0f);  // Red

			UpdateText("fps_text", std::to_string(fps));
			ECS::GetInstance().GetSystem<Renderer>()->UpdateTextColor("fps_text", fpsColor);
		}
		else
		{
			UpdateText("fps_text", "");
		}
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
	void InGameGUI::CreateText(const std::string& text_id, const std::string& label, const Vec2& pos, const float scale, const Vec3& color, const std::string& font_name)
	{
		ECS::GetInstance().GetSystem<Renderer>()->CreateTextObject(text_id, label, pos, scale, color, font_name);
	}

	/*!***********************************************************************
	\brief
	 Update the label of an existing GUI text object.
	\param[in] id
	 The ID of the text object to update.
	\param[in] new_label
	 The new text to set as the label.
	*************************************************************************/
	void InGameGUI::UpdateText(const std::string& text_id, const std::string& new_label)
	{
		ECS::GetInstance().GetSystem<Renderer>()->UpdateTextObject(text_id, new_label);
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
		auto uiManager = ECS::GetInstance().GetSystem<UIButtonManager>();
		Application& app = Application::Get(); 
		int screen_width = app.GetWindow().GetWidth(); 
		int screen_height = app.GetWindow().GetHeight(); 

		//--Health---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
		uiManager->addButton("health bar bg", glm::vec3(353.f, 1000.f, 0.f), glm::vec2(627.f, 66.f), "in game_health bar bg", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, false, []() {
			});
		uiManager->addButton("health bar", glm::vec3(353.f, 1000.f, 0.f), glm::vec2(627.f, 66.f), "in game_health bar", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::Health, false, []() {
			});
		uiManager->addButton("health bar border", glm::vec3(356.f, 1003.f, 0.f), glm::vec2(598.f, 36.f), "in game_health bar border", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, false, []() {
			});
		uiManager->addButton("health bar icon", glm::vec3(53.f, 1000.f, 0.f), glm::vec2(117.f, 129.f), "in game_health bar icon", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, false, []() {
			});
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		//--Soul blue----------------------------------------------------------------------------------------------------------------------------------------------------------------------
		uiManager->addButton("blue soul bar bg", glm::vec3(300.f, 937.f, 0.f), glm::vec2(409.5f, 55.f), "in game_soul bar bg", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, false, []() {
			});

		uiManager->addButton("blue soul bar", glm::vec3(300.f, 943.f, 0.f), glm::vec2(357.f, 13.f), "in game_soul bar blue", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::Blue_Soul, false, []() {
			});

		uiManager->addButton("blue soul bar border", glm::vec3(302.f, 941.f, 0.f), glm::vec2(360.f, 20.f), "in game_soul bar border", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 2, BarType::None, false, []() {
			});

		uiManager->addButton("blue fire", glm::vec3(130.f, 943.f, 0.f), glm::vec2(65.f, 71.f), "in game_blue fire", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 2, BarType::None, false, []() {
			});

		// Ability bar
		uiManager->addButton("blue soul ability charge bg", glm::vec3(499.f, 938.f, 0.f), glm::vec2(58.f, 61.f), "in game_soul ability charge bg", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, false, []() {
			});

		uiManager->addButton("blue soul ability charge bar", glm::vec3(500.f, 942.f, 0.f), glm::vec2(25.f, 27.f), "in game_soul ability charge blue", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::Blue_Charge_Bar, false, []() {
			});

		uiManager->addButton("blue soul ability charge border", glm::vec3(500.5f, 942.f, 0.f), glm::vec2(31.f, 31.f), "in game_soul ability charge border", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, false, []() {
			});
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		//--Soul red----------------------------------------------------------------------------------------------------------------------------------------------------------------------
		uiManager->addButton("red soul bar bg", glm::vec3(330.5f, 879.f, 0.f), glm::vec2(409.5f, 55.f), "in game_soul bar bg", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, false, []() {
			});

		uiManager->addButton("red soul bar", glm::vec3(333.f, 885.f, 0.f), glm::vec2(357.f, 13.f), "in game_soul bar red", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::Red_Soul, false, []() {
			});

		uiManager->addButton("red soul bar border", glm::vec3(333.f, 883.f, 0.f), glm::vec2(360.f, 20.f), "in game_soul bar border", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, false, []() {
			});

		uiManager->addButton("red fire", glm::vec3(163.f, 883.f, 0.f), glm::vec2(65.f, 71.f), "in game_red fire", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 2, BarType::None, false, []() {
			});

		// Ability bar
		uiManager->addButton("red soul ability charge bg", glm::vec3(532.f, 877.f, 0.f), glm::vec2(58.f, 61.f), "in game_soul ability charge bg", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, false, []() {
			});

		uiManager->addButton("red soul ability charge bar", glm::vec3(533.f, 881.f, 0.f), glm::vec2(25.f, 27.f), "in game_soul ability charge red", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::Red_Charge_Bar, false, []() {
			});

		uiManager->addButton("red soul ability charge border", glm::vec3(534.f, 881.f, 0.f), glm::vec2(31.f, 31.f), "in game_soul ability charge border", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, false, []() {
			});
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		//--Ability and Pause----------------------------------------------------------------------------------------------------------------------------------------------------------------------
		uiManager->addButton("soul change", glm::vec3(1825.f, 100.f, 0.f), glm::vec2(119.f, 121.f), "in game_soul change", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, false, []() {
			});

		uiManager->addButton("game ability", glm::vec3(1675.f, 100.f, 0.f), glm::vec2(119.f, 121.f), "in game_abilities", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, false, []() {
			});

		uiManager->addButton("pause button", glm::vec3(1825.f, 1000.f, 0.f), glm::vec2(119.f, 121.f), "in game_pause", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 1, BarType::None, false, []() {
			});
		//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

		//// Main Menu
		//uiManager->addButton("main menu", glm::vec3{ screen_width * 0.5f, screen_height * 0.5f , 0.f }, glm::vec2{ static_cast<float>(screen_width), static_cast<float>(screen_height) }, "ui_mainmenu", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 2, BarType::None, false, []() {
		//	// Get the AudioManager
		//	auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();

		//	// Start playing main menu music if it exists
		//	if (audioM.GetMusicIndex("BGMOG") != -1)
		//	{
		//		if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsMusicPlaying(audioM.GetMusicIndex("BGMOG")))
		//		{
		//			audioM.PlayMusic(audioM.GetMusicIndex("BGMOG"));
		//		}
		//	}
		//	});

		
	}

	void InGameGUI::CreateMainMenuUI()
	{
		auto uiManager = ECS::GetInstance().GetSystem<UIButtonManager>();

		uiManager->addButton("start button", glm::vec3{ 1138.f, 538.f, 0.f }, glm::vec2{ 422.f, 343.f }, "start button", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 3, BarType::None, true, []() {
			// Get the AudioManager
			auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();

			// Stop the main menu music when starting the game
			if (audioM.GetMusicIndex("BGMOG") != -1)
			{
				audioM.StopMusic(audioM.GetMusicIndex("BGMOG"));
			}

			// Start the game
			Application::Get().StartGame();

			ECS::GetInstance().GetSystem<UIButtonManager>()->removeButton("credit button");
			ECS::GetInstance().GetSystem<UIButtonManager>()->removeButton("exit button");
			// Check if the StartButton SFX exists and play it
			if (audioM.GetSFXindex("ButtonClickSound") != -1)
			{
				audioM.PlaySFX(audioM.GetSFXindex("ButtonClickSound"));
			}

			ECS::GetInstance().GetSystem<VideoManager>()->SkipVideo();
			ECS::GetInstance().GetSystem<VideoManager>()->videos["main_menu"].done = true;
			});

		uiManager->addButton("credit button", glm::vec3{1538.f, 488.f, 0.f }, glm::vec2{ 238.f, 169.f }, "credits button", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 3, BarType::None, true, [this]() {
			auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
			if (audioM.GetSFXindex("ButtonClickSound") != -1)
			{
				audioM.PlaySFX(audioM.GetSFXindex("ButtonClickSound"));
			}
			
			this->ShowCredits();
			showCredits = true;
			});

		uiManager->addButton("exit button", glm::vec3{ 1738.f, 438.f, 0.f }, glm::vec2{ 101.f, 168.f }, "exit button", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 3, BarType::None, true, []() {
			auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
			if (audioM.GetSFXindex("ButtonClickSound") != -1)
			{
				audioM.PlaySFX(audioM.GetSFXindex("ButtonClickSound"));
			}
			Application::Get().QuitGame();
			});
	}

	void InGameGUI::ShowCredits()
	{
		// Hide main menu buttons
		ECS::GetInstance().GetSystem<UIButtonManager>()->removeButton("start button");
		ECS::GetInstance().GetSystem<UIButtonManager>()->removeButton("credit button");
		ECS::GetInstance().GetSystem<UIButtonManager>()->removeButton("exit button");

		auto uiManager = ECS::GetInstance().GetSystem<UIButtonManager>();

		// Add the credits background image
		uiManager->addButton("credits", glm::vec3{ 960.f, 540.f + -(7086.f * 0.5f) + (1080.f * 0.5f) + 540.f, 0.f }, glm::vec2{ 1920.f, 7086.f },
			"credits", glm::vec3(1.0f, 1.0f, 1.0f),
			ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 2, BarType::None, false, []() {});

		// Add the back button to return to main menu
		
	}

	void InGameGUI::UpdateCredits()
	{
		float deltaTime = g_FrameRateController.GetDeltaTime();

		//static float scrollSpeed = 100.0f; // Adjust speed as needed

		auto button = ECS::GetInstance().GetSystem<UIButtonManager>()->getButtonByID("credits");
		if (button)
		{
			button->position.y += scrollSpeed * deltaTime; // Move UP

			if (button->position.y >= 540.f + (7086.f * 0.5f) - (1080.f * 0.5f))
			{
				// Reset to start position
				button->position.y = 540.f + -(7086.f * 0.5f) + (1080.f * 0.5f) + 540.f;  // Reset to bottom (screen height)
			}
		}
	}


	/*!***********************************************************************
	\brief
	 Remove a GUI object.
	\param[in] id
	 The ID for the GUI object.
	*************************************************************************/
	void InGameGUI::RemoveElement(const std::string& text_id)
	{
		ECS::GetInstance().GetSystem<UIButtonManager>()->removeButton(text_id);
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
		auto [mouseX, mouseY] = Input::GetMousePosition();
		mouseY = ECS::GetInstance().GetSystem<Camera>()->viewport_size.y - mouseY; // Flip Y-axis

		auto& buttons = ECS::GetInstance().GetSystem<UIButtonManager>()->buttons;

		// Adjusted to use shared_ptr correctly
		std::vector<std::pair<std::string, std::shared_ptr<UIButton>>> sortedButtons(buttons.begin(), buttons.end());

		// Sort buttons by UI layer (descending order)
		std::sort(sortedButtons.begin(), sortedButtons.end(), [](const auto& a, const auto& b) {
			return a.second->ui_layer > b.second->ui_layer;  // Higher layer first
			});

		auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
		// Update hover cooldown timer
		hoverTimer -= static_cast<float>(g_FrameRateController.GetDeltaTime());

		// Handle mouse click for the highest-layer button
		for (auto& [button_id, button] : sortedButtons) 
		{
			button->isHovered = IsInside(Vec2(button->position.x, button->position.y), Vec2(button->size.x, button->size.y));

			// Check if hover state has changed
			if (ECS::GetInstance().GetSystem<VideoManager>()->IsVideoDonePlaying("cutscene") && button->isHovered && !buttonHoverState[button_id] && hoverTimer <= 0.0f)
			{
				if (audioM.GetSFXindex("HoverSound") != -1)
				{
					audioM.PlaySFX(audioM.GetSFXindex("HoverSound"));
				}
				hoverTimer = hoverCooldown; // Reset cooldown timer
			}

			// Update hover state
			buttonHoverState[button_id] = button->isHovered;

			// Check for mouse click
			if (ECS::GetInstance().GetSystem<VideoManager>()->IsVideoDonePlaying("cutscene") && button->isHovered && Input::IsMouseButtonPressed(UME_MOUSE_BUTTON_1))
			{
				if (button->onClick)
				{
					button->onClick();
					break; // Stop after handling the first clickable button
				}
			}
		}

		// Pause
		if (Input::IsKeyTriggered(UME_KEY_ESCAPE) && Application::Get().GameStarted)
		{
			auto& rButton = ECS::GetInstance().GetSystem<UIButtonManager>()->buttons["pause button"];
			if (rButton)
			{
				rButton->triggerDarkenEffect();
			}
			bool newPauseState = !Application::Get().Paused();
			Application::Get().SetPaused(newPauseState);

			if (newPauseState)
			{
				ShowPauseMenu();
			}
			
		}
		if (Input::IsKeyTriggered(UME_KEY_M))
			Application::Get().QuitGame();

		// Press enter to start game
		if (!Application::Get().GameStarted && Input::IsKeyTriggered(UME_KEY_ENTER))
		{
			Application::Get().StartGame();
		}

		// Press F10 to toggle fps text
		if (Input::IsKeyTriggered(UME_KEY_F10))
			show_fps = !show_fps;
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

	/*!***********************************************************************
	\brief
	 Displays the pause menu by adding necessary UI elements.
	*************************************************************************/
	void InGameGUI::ShowPauseMenu()
	{
		Application& app = Application::Get();
		int screen_width = app.GetWindow().GetWidth();
		int screen_height = app.GetWindow().GetHeight();
		auto uiManager = ECS::GetInstance().GetSystem<UIButtonManager>();

		uiManager->addButton("pause overlay", glm::vec3(screen_width * 0.5f, screen_height * 0.5f, 0.f), glm::vec2((float)screen_width, (float)screen_height), "overlay bg", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 6);

		uiManager->addButton("pause bg", glm::vec3(screen_width * 0.5f, screen_height * 0.5f, 0.f), glm::vec2(999.f, 869.f), "back", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 7);
		uiManager->addButton("pause", glm::vec3(screen_width * 0.5f, (screen_height * 0.5f) + 200.f, 0.f), glm::vec2(511.f, 131.f), "pause", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 7);

		// Resume
		uiManager->addButton("resume button bg", glm::vec3(810.f, 550.f, 0.f), glm::vec2(147.f, 134.f), "button2", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 8, BarType::None, true, []() {
			});
		uiManager->addButton("resume button", glm::vec3(810.f, 550.f, 0.f), glm::vec2(73.f, 86.f), "return icon", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 9, BarType::None, true, [this]() {
			auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();

			// Check if the StartButton SFX exists and play it
			if (audioM.GetSFXindex("ButtonClickSound") != -1)
			{
				audioM.PlaySFX(audioM.GetSFXindex("ButtonClickSound"));
			}
			Application::Get().SetPaused(false);
			this->HidePauseMenu();
			});
		uiManager->addButton("resume text", glm::vec3(810.f, 450.f, 0.f), glm::vec2(118.f, 26.f), "return", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 8, BarType::None);

		// Exit
		uiManager->addButton("exit button bg", glm::vec3(1110.f, 550.f, 0.f), glm::vec2(145.f, 135.f), "button1", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 8, BarType::None, true, []() {
			});
		uiManager->addButton("exit button", glm::vec3(1110.f, 550.f, 0.f), glm::vec2(75.f, 85.f), "exit icon", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 10, BarType::None, true, [this]() {
			//Application::Get().StopGame();  // Stop the game
			//Application::Get().SetPaused(false);  // Ensure it's unpaused
			auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
			if (audioM.GetSFXindex("ButtonClickSound") != -1)
			{
				audioM.PlaySFX(audioM.GetSFXindex("ButtonClickSound"));
			}
			Application::Get().QuitGame();
			});
		uiManager->addButton("exit text", glm::vec3(1110.f, 450.f, 0.f), glm::vec2(74.f, 35.f), "exit", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 11, BarType::None);
	}

	/*!***********************************************************************
	\brief
	 Removes all pause menu UI elements from the screen.
	*************************************************************************/
	void InGameGUI::HidePauseMenu()
	{
		auto uiManager = ECS::GetInstance().GetSystem<UIButtonManager>();

		// Remove all pause menu elements
		uiManager->removeButton("pause overlay");
		uiManager->removeButton("pause bg");
		uiManager->removeButton("pause");
		uiManager->removeButton("resume button bg");
		uiManager->removeButton("resume button");
		uiManager->removeButton("resume text");
		uiManager->removeButton("exit button");
		uiManager->removeButton("exit button bg");
		uiManager->removeButton("exit text");
	}

	void InGameGUI::showDefeatScreen()
	{
		Application& app = Application::Get();
		int screen_width = app.GetWindow().GetWidth();
		int screen_height = app.GetWindow().GetHeight();
		auto uiManager = ECS::GetInstance().GetSystem<UIButtonManager>();

		uiManager->addButton("defeat overlay", glm::vec3(screen_width * 0.5f, screen_height * 0.5f, 0.f), glm::vec2((float)screen_width, (float)screen_height), "overlay2", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 3);
		uiManager->addButton("defeat text", glm::vec3(screen_width * 0.5f, 800.f, 0.f), glm::vec2(499.f, 129.f), "defeat", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 5);
		uiManager->addButton("defeat bg", glm::vec3(screen_width * 0.5f, screen_height * 0.5f, 0.f), glm::vec2(1920.f, 1080.f), "defeat bg", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 4);
		// Restart
		uiManager->addButton("restart bg", glm::vec3(810.f, 350.f, 0.f), glm::vec2(145.f, 135.f), "button1", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 5, BarType::None, true, [this]() {
			//pause = false;
			this->HideDefeatScreen();
			});
		uiManager->addButton("restart button", glm::vec3(810.f, 350.f, 0.f), glm::vec2(84.f, 88.f), "restart icon", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 6, BarType::None, true, [this]() {
			pause = false;
			std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
			this->HideDefeatScreen();
			});
		uiManager->addButton("restart button text", glm::vec3(810.f, 250.f, 0.f), glm::vec2(140.f, 27.f), "restart", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 4);

		// Exit
		uiManager->addButton("exit button bg", glm::vec3(1160.f, 350.f, 0.f), glm::vec2(145.f, 135.f), "button2", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 5, BarType::None, true, [this]() {
			//Application::Get().IsPaused = false;
			pause = false;
			ECS::GetInstance().GetSystem<InGameGUI>()->HideDefeatScreen();
			});
		uiManager->addButton("exit button", glm::vec3(1160.f, 350.f, 0.f), glm::vec2(75.f, 85.f), "exit icon", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 6, BarType::None, true, []() {
			});
		uiManager->addButton("exit text", glm::vec3(1160.f, 250.f, 0.f), glm::vec2(74.f, 35.f), "exit", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 4, BarType::None);
	}

	void InGameGUI::HideDefeatScreen()
	{
		auto uiManager = ECS::GetInstance().GetSystem<UIButtonManager>();

		// Remove defeat screen elements
		uiManager->removeButton("defeat overlay");
		uiManager->removeButton("defeat text");
		uiManager->removeButton("defeat bg");
		uiManager->removeButton("restart bg");
		uiManager->removeButton("restart button");
		uiManager->removeButton("restart button text");
		uiManager->removeButton("exit button bg");
		uiManager->removeButton("exit button");
		uiManager->removeButton("exit text");
	}

}