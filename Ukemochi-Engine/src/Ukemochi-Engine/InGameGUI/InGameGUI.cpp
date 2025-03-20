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
#include "../SceneManager.h"

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
		//CreateMainMenuUI();
		// Create FPS text
		CreateText("fps_text", "", Vec2{ screen_width * 0.92f, screen_height * 0.82f }, 1.5f, Vec3{ 1.f, 1.f, 1.f }, "Ukemochi_numbers");
	}

	/*!***********************************************************************
	\brief
	 Update the inputs of the in game GUI system.
	*************************************************************************/
	void InGameGUI::Update()
	{
		// Handle button inputs
		HandleButtonInput();

		UpdateMusicBar();

		UpdateSFXBar();

		//auto& player = ECS::GetInstance().GetComponent<Player>(ECS::GetInstance().GetSystem<Renderer>()->getPlayerID());

		GameObject* playerObj = GameObjectManager::GetInstance().GetGOByTag("Player");

		if (playerObj == nullptr)
		{
			return;
		}

		auto& player = playerObj->GetComponent<Player>();

		// Check if player health is 0
		if (player.currentHealth <= 0)
		{
			// Start the timer
			deathTimer += (float)g_FrameRateController.GetDeltaTime();

			// If the timer exceeds the delay, show the defeat screen
			if (deathTimer >= deathScreenDelay)
			{
				// Pause the game once
				if (!Application::Get().Paused())
				{
					Application::Get().SetPaused(true);
				}

				// Show the defeat screen
				showDefeatScreen();
				deathTimer = 0.f;
			}
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
	 Create a GUI button object.
	*************************************************************************/
	void InGameGUI::CreateImage()
	{
		auto uiManager = ECS::GetInstance().GetSystem<UIButtonManager>();

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

	}

	void InGameGUI::CreateMainMenuUI()
	{
		auto uiManager = ECS::GetInstance().GetSystem<UIButtonManager>();

		uiManager->addButton("start button", glm::vec3{ 1175.f, 508.f, 0.f }, glm::vec2{ 422.f, 343.f }, "start button", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 3, BarType::None, true, []() {
			
			if(GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
			{
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
				ECS::GetInstance().GetSystem<UIButtonManager>()->removeButton("title");
				ECS::GetInstance().GetSystem<UIButtonManager>()->removeButton("nail start");
				ECS::GetInstance().GetSystem<UIButtonManager>()->removeButton("nail credit");
				ECS::GetInstance().GetSystem<UIButtonManager>()->removeButton("nail exit");

				// Check if the StartButton SFX exists and play it
				if (audioM.GetSFXindex("ButtonClickSound") != -1)
				{
					audioM.PlaySFX(audioM.GetSFXindex("ButtonClickSound"));
				}

				ECS::GetInstance().GetSystem<VideoManager>()->SkipVideo();
				ECS::GetInstance().GetSystem<VideoManager>()->videos["main_menu"].done = true;
			}
			});

		uiManager->addButton("credit button", glm::vec3{1532.f, 488.f, 0.f }, glm::vec2{ 238.f, 169.f }, "credits button", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 3, BarType::None, true, [this]() {
			if(GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
			{
				auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
				if (audioM.GetSFXindex("ButtonClickSound") != -1)
				{
					audioM.PlaySFX(audioM.GetSFXindex("ButtonClickSound"));
				}
				
				this->ShowCredits();
				showCredits = true;
			}
			});

		uiManager->addButton("exit button", glm::vec3{ 1663.f, 338.f, 0.f }, glm::vec2{ 101.f, 168.f }, "exit button", glm::vec3(1.0f, 1.0f, 1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 3, BarType::None, true, []() {
			if(GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
			{
				auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
				if (audioM.GetSFXindex("ButtonClickSound") != -1)
				{
					audioM.PlaySFX(audioM.GetSFXindex("ButtonClickSound"));
				}
				Application::Get().QuitGame();
			}
			});

		uiManager->addButton("title", glm::vec3{ 1440.f, 1300.f, 0.f }, glm::vec2{ 845.6f, 380.f },
			"logo", glm::vec3(1.0f, 1.0f, 1.0f),
			ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 2, BarType::None, false, []() {});

		uiManager->addButton("nail start", glm::vec3{ 1175.f, 508.f, 0.f }, glm::vec2{ 26.f, 24.f },
			"nail", glm::vec3(1.0f, 1.0f, 1.0f),
			ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 3, BarType::None, false, []() {});

		uiManager->addButton("nail credit", glm::vec3{ 1532.f, 488.f, 0.f }, glm::vec2{ 26.f, 24.f },
			"nail", glm::vec3(1.0f, 1.0f, 1.0f),
			ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 3, BarType::None, false, []() {});

		uiManager->addButton("nail exit", glm::vec3{ 1652.f, 338.f, 0.f }, glm::vec2{ 26.f, 24.f },
			"nail", glm::vec3(1.0f, 1.0f, 1.0f),
			ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 4, BarType::None, false, []() {});
	}

	/*!***********************************************************************
	\brief
	 Displays the credits by adding necessary UI elements.
	*************************************************************************/
	void InGameGUI::ShowCredits()
	{
		// Hide main menu buttons
		RemoveElement("start button"); 
		RemoveElement("credit button"); 
		RemoveElement("exit button"); 
		RemoveElement("title");
		RemoveElement("nail start");
		RemoveElement("nail credit");
		RemoveElement("nail exit");

		auto uiManager = ECS::GetInstance().GetSystem<UIButtonManager>();

		// Add the credits background image
		uiManager->addButton("credits", glm::vec3{ 960.f, 540.f + -(6962.f * 0.5f) + (1080.f * 0.5f) + 540.f, 0.f }, glm::vec2{ 1921.f, 7086.f },
			"credits", glm::vec3(1.0f, 1.0f, 1.0f),
			ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 2, BarType::None, false, []() {});

		// Add the back button to return to main menu
		uiManager->addButton("main menu return", glm::vec3{ 60.f, 1025.f, 0.f }, glm::vec2{ 73.f, 86.f },
			"Group 4", glm::vec3(1.0f, 1.0f, 1.0f),
			ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 4, BarType::None, false, [this]() {
				if(GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
				{
					auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
					if (audioM.GetSFXindex("ButtonClickSound") != -1)
					{
						audioM.PlaySFX(audioM.GetSFXindex("ButtonClickSound"));
					}
					
					this->RemoveElement("credits");
					this->RemoveElement("main menu return");

					this->CreateMainMenuUI();
				}
			});
	}

	/*!***********************************************************************
	 * @brief Updates the scrolling credits animation.
	 *
	 * This function moves the credits screen upwards over time.
	 * When the credits reach the top of the screen, it resets them to the start position.
	 *************************************************************************/
	void InGameGUI::UpdateCredits()
	{
		float deltaTime = (float)g_FrameRateController.GetDeltaTime();

		//static float scrollSpeed = 100.0f; // Adjust speed as needed

		auto button = ECS::GetInstance().GetSystem<UIButtonManager>()->getButtonByID("credits");
		if (button)
		{
			button->position.y += scrollSpeed * deltaTime; // Move UP

			if (button->position.y >= 540.f + (6962.f * 0.5f) - (1080.f * 0.5f))
			{
				// Reset to start position
				button->position.y = 540.f + -(6962.f * 0.5f) + (1080.f * 0.5f) + 540.f;  // Reset to bottom (screen height)
			}
		}
	}

	/*!***********************************************************************
	 * @brief Handles the title animation, making it fall into place.
	 *
	 * The title starts above the screen and falls until it reaches its target position.
	 * This function gradually moves the title downwards and clamps it to the final position.
	 *************************************************************************/
	void InGameGUI::UpdateTitleAnimation()
	{
		auto uiManager = ECS::GetInstance().GetSystem<UIButtonManager>();
		auto titleButton = uiManager->getButtonByID("title");

		float deltaTime = (float)g_FrameRateController.GetDeltaTime();

		if (!titleButton) return; // Ensure the button exists

		float targetY = 850.f; // Final position
		float fallSpeed = 600.f; // Pixels per second

		// Move down until it reaches the target position
		if (titleButton->position.y > targetY)
		{
			titleButton->position.y -= fallSpeed * deltaTime;

			// Clamp the position so it doesn't go past the target
			if (titleButton->position.y < targetY)
				titleButton->position.y = targetY;
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

	/*!***********************************************************************
	\brief
	 Removes a text object from the UI.
	*************************************************************************/
	void InGameGUI::RemoveText(const std::string& textid)
	{
		ECS::GetInstance().GetSystem<Renderer>()->RemoveTextObject(textid);
	}

	/*!***********************************************************************
	\brief
	 Renders all UI elements.
	\param[in] cameraPos
	 The camera position to correctly position UI elements.
	*************************************************************************/
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


		// Update hover cooldown timer
		hoverTimer -= static_cast<float>(g_FrameRateController.GetDeltaTime());

		// Handle mouse click for the highest-layer button
		for (auto& [button_id, button] : sortedButtons)
		{
			button->isHovered = IsInside(Vec2(button->position.x, button->position.y), Vec2(button->size.x, button->size.y));

			// Check if hover state has changed
			if (ECS::GetInstance().GetSystem<VideoManager>()->IsVideoDonePlaying("cutscene") && button->isHovered && !buttonHoverState[button_id] && hoverTimer <= 0.0f)
			{
				if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
				{
					auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
					if (audioM.GetSFXindex("HoverSound") != -1)
					{
						audioM.PlaySFX(audioM.GetSFXindex("HoverSound"));
					}
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
				// Hide stats when pausing if they were showing
				if (show_stats)
				{
					show_stats = false;
					HideStats();
				}
			}
		}

		if (Input::IsKeyTriggered(UME_KEY_M))
			Application::Get().QuitGame();

		// Press F10 to toggle fps text
		if (Input::IsKeyTriggered(UME_KEY_F10))
			show_fps = !show_fps;

		// Press P to show stats - only if game is not paused
		if (Input::IsKeyTriggered(UME_KEY_P) && Application::Get().GameStarted && !Application::Get().Paused())
		{
			show_stats = !show_stats;
			if (show_stats)
				ShowStats();
			else
				HideStats();
		}
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
		uiManager->addButton("resume button", glm::vec3(810.f, 550.f, 0.f), glm::vec2(147.f, 172.f), "return", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 8, BarType::None, true, [this]() {
			
			if(GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
			{

				auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();

				// Check if the StartButton SFX exists and play it
				if (audioM.GetSFXindex("ButtonClickSound") != -1)
				{
					audioM.PlaySFX(audioM.GetSFXindex("ButtonClickSound"));
				}
				Application::Get().SetPaused(false);
				this->HidePauseMenu();
			}
			});

		// Exit
		uiManager->addButton("exit button", glm::vec3(1110.f, 550.f, 0.f), glm::vec2(145.f, 175.f), "exit", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 8, BarType::None, true, [this]() {
			if(GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
			{
				auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
				if (audioM.GetSFXindex("ButtonClickSound") != -1)
				{
					audioM.PlaySFX(audioM.GetSFXindex("ButtonClickSound"));
				}
				Application::Get().QuitGame();
			}
			});

		static float lastClickTime = 0.0f;

		// Music
		uiManager->addButton("music", glm::vec3(630.f, 350.f, 0.f), glm::vec2(51.f, 53.f), "music", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 8, BarType::None, false, []() {});

		uiManager->addButton("music minus", glm::vec3(710.f, 350.f, 0.f), glm::vec2(59.f, 62.f), "minus", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 8, BarType::None, true, [this]() {
			float currentTime = (float)glfwGetTime();
			if (GameObjectManager::GetInstance().GetGOByTag("AudioManager") && (currentTime - lastClickTime) > 0.2f) {
				auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
				audioM.SetMusicVolume(-0.1f);
				lastClickTime = currentTime; // Update last click time
				// Check if the StartButton SFX exists and play it
				if (audioM.GetSFXindex("ButtonClickSound") != -1)
				{
					audioM.PlaySFX(audioM.GetSFXindex("ButtonClickSound"));
				}
			}
			});

		uiManager->addButton("music plus", glm::vec3(1210.f, 350.f, 0.f), glm::vec2(58.f, 63.f), "plus", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 8, BarType::None, true, [this]() {
			float currentTime = (float)glfwGetTime();
			if (GameObjectManager::GetInstance().GetGOByTag("AudioManager") && (currentTime - lastClickTime) > 0.2f) {
				auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
				audioM.SetMusicVolume(0.1f);
				lastClickTime = currentTime; // Update last click time
				if (audioM.GetSFXindex("ButtonClickSound") != -1)
				{
					audioM.PlaySFX(audioM.GetSFXindex("ButtonClickSound"));
				}
			}
			});

		uiManager->addButton("music bar bg", glm::vec3(960.f, 350.f, 0.f), glm::vec2(429.f, 27.f), "Rounded Rectangle 8", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 8, BarType::None, false, []() {
			
			});

		uiManager->addButton("music bar", glm::vec3(960.f, 350.f, 0.f), glm::vec2(429.f, 27.f), "Rounded Rectangle 8 3", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 9, BarType::None, false, []() {
			
			});

		// SFX
		uiManager->addButton("sfx", glm::vec3(630.f, 250.f, 0.f), glm::vec2(51.f, 53.f), "music2", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 8, BarType::None, false, []() {});

		uiManager->addButton("sfx minus", glm::vec3(710.f, 250.f, 0.f), glm::vec2(59.f, 62.f), "minus", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 8, BarType::None, true, [this]() {
			float currentTime = (float)glfwGetTime();
			if (GameObjectManager::GetInstance().GetGOByTag("AudioManager") && (currentTime - lastClickTime) > 0.2f) {
				auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
				audioM.SetSFXvolume(-0.1f);
				lastClickTime = currentTime; // Update last click time
				if (audioM.GetSFXindex("ButtonClickSound") != -1)
				{
					audioM.PlaySFX(audioM.GetSFXindex("ButtonClickSound"));
				}
			}
			});

		uiManager->addButton("sfx plus", glm::vec3(1210.f, 250.f, 0.f), glm::vec2(58.f, 63.f), "plus", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 8, BarType::None, true, [this]() {
			float currentTime = (float)glfwGetTime();
			if (GameObjectManager::GetInstance().GetGOByTag("AudioManager") && (currentTime - lastClickTime) > 0.2f) {
				auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
				audioM.SetSFXvolume(0.1f);
				lastClickTime = currentTime; // Update last click time
				if (audioM.GetSFXindex("ButtonClickSound") != -1)
				{
					audioM.PlaySFX(audioM.GetSFXindex("ButtonClickSound"));
				}
			}
			});

		uiManager->addButton("sfx bar bg", glm::vec3(960.f, 250.f, 0.f), glm::vec2(429.f, 27.f), "Rounded Rectangle 8", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 8, BarType::None, false, []() {

			});

		uiManager->addButton("sfx bar", glm::vec3(960.f, 250.f, 0.f), glm::vec2(429.f, 27.f), "Rounded Rectangle 8 3", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 9, BarType::None, false, []() {

			});
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

		uiManager->removeButton("resume button");

		uiManager->removeButton("exit button");

		uiManager->removeButton("music");
		uiManager->removeButton("music minus");
		uiManager->removeButton("music plus");
		uiManager->removeButton("music bar bg");
		uiManager->removeButton("music bar");

		uiManager->removeButton("sfx");
		uiManager->removeButton("sfx minus");
		uiManager->removeButton("sfx plus");
		uiManager->removeButton("sfx bar bg");
		uiManager->removeButton("sfx bar");
	}

	/*!***********************************************************************
	\brief
	 Displays the defeat screen when the player loses.
	*************************************************************************/
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
		uiManager->addButton("restart button", glm::vec3(810.f, 350.f, 0.f), glm::vec2(147.f, 177.f), "restart", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 6, BarType::None, true, [this]() {

			std::this_thread::sleep_for(std::chrono::milliseconds(50)); 
			this->HideDefeatScreen();

			if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
			{
				if (GameObjectManager::GetInstance().GetGOByTag("AudioManager")->HasComponent<AudioManager>())
				{
					auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
					audioM.StopMusic(audioM.GetMusicIndex("BGM"));
				}
			}
			
			SceneManager::GetInstance().ResetGame();
			CreateImage();
			Application::Get().SetPaused(false);
			});

		// Exit
		uiManager->addButton("exit button defeat", glm::vec3(1160.f, 350.f, 0.f), glm::vec2(145.f, 175.f), "exit", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 6, BarType::None, true, []() {
			Application::Get().QuitGame();
			});
	}

	/*!***********************************************************************
	\brief
	 Removes all defeat screen UI elements from the screen.
	*************************************************************************/
	void InGameGUI::HideDefeatScreen()
	{
		auto uiManager = ECS::GetInstance().GetSystem<UIButtonManager>();

		// Remove defeat screen elements
		uiManager->removeButton("defeat overlay");
		uiManager->removeButton("defeat text");
		uiManager->removeButton("defeat bg");
		
		uiManager->removeButton("restart button");

		uiManager->removeButton("exit button defeat");
	}

	/*!***********************************************************************
	\brief
	 Displays the stats screen.
	*************************************************************************/
	void InGameGUI::ShowStats()
	{
		auto uiManager = ECS::GetInstance().GetSystem<UIButtonManager>(); 
		uiManager->addButton("stats", glm::vec3(960.f, 540.f, 0.f), glm::vec2(1419.f, 875.f), "stat", glm::vec3(1.0f), ECS::GetInstance().GetSystem<Renderer>()->batchRendererUI, 8, BarType::None, false, []() {
			});

		auto& player = ECS::GetInstance().GetComponent<Player>(ECS::GetInstance().GetSystem<Renderer>()->getPlayerID());
		CreateText("health", std::to_string(player.currentHealth), Vec2(1445.f, 585.f), 1.5f, Vec3(0.0f, 0.0f, 0.0f), "Ukemochi_numbers");

		CreateText("attack", std::to_string(player.comboDamage), Vec2(1195.f, 585.f), 1.5f, Vec3(0.0f, 0.0f, 0.0f), "Ukemochi_numbers");

		CreateText("armour", std::to_string(5), Vec2(1195.f, 485.f), 1.5f, Vec3(0.0f, 0.0f, 0.0f), "Ukemochi_numbers");

		CreateText("movement speed", std::to_string((int)(player.playerForce / 1000.f)), Vec2(1445.f, 485.f), 1.5f, Vec3(0.0f, 0.0f, 0.0f), "Ukemochi_numbers");
	}

	/*!***********************************************************************
	\brief
	 Removes all stats screen UI elements from the screen.
	*************************************************************************/
	void InGameGUI::HideStats()
	{
		auto uiManager = ECS::GetInstance().GetSystem<UIButtonManager>();
		uiManager->removeButton("stats");
		
		RemoveText("health");
		RemoveText("attack");
		RemoveText("armour");
		RemoveText("movement speed");
	}

	/*!***********************************************************************
	\brief
	 Updates the music volume bar UI based on the current audio volume level.

	\details
	 This function retrieves the current music volume from the AudioManager
	 and adjusts the width and position of the "music bar" UI element
	 to visually represent the current volume level.

	\remarks
	 If the AudioManager is not found or the music list is empty, the bar
	 remains unchanged.

	*************************************************************************/
	void InGameGUI::UpdateMusicBar()
	{
		auto uiManager = ECS::GetInstance().GetSystem<UIButtonManager>();

		// Get the button reference
		auto musicBar = uiManager->getButtonByID("music bar");
		if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
		{
			if (GameObjectManager::GetInstance().GetGOByTag("AudioManager")->HasComponent<AudioManager>())
			{
				auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
				
				if (musicBar)
				{
					float volume = audioM.music.empty() ? 0.0f : audioM.music[0].volume;  // Get the current volume
					float maxWidth = 429.0f;  // Max width of the volume bar

					// Update size and position dynamically
					musicBar->size.x = maxWidth * volume;
					musicBar->position.x = 960.f - (maxWidth - musicBar->size.x) * 0.5f;
				}
			}
		}
	}

	/*!***********************************************************************
	\brief
	 Updates the SFX volume bar UI based on the current audio volume level.

	\details
	 This function retrieves the current SFX volume from the AudioManager
	 and adjusts the width and position of the "sfx bar" UI element
	 to visually represent the current SFX volume level.

	\remarks
	 If the AudioManager is not found or the SFX list is empty, the bar
	 remains unchanged.

	*************************************************************************/
	void InGameGUI::UpdateSFXBar()
	{
		auto uiManager = ECS::GetInstance().GetSystem<UIButtonManager>();

		// Get the button reference
		auto sfxBar = uiManager->getButtonByID("sfx bar");
		if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
		{
			if (GameObjectManager::GetInstance().GetGOByTag("AudioManager")->HasComponent<AudioManager>())
			{
				auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();

				if (sfxBar)
				{
					float volume = audioM.sfx.empty() ? 0.0f : audioM.sfx[0].volume;  // Get the current volume
					float maxWidth = 429.0f;  // Max width of the volume bar

					// Update size and position dynamically
					sfxBar->size.x = maxWidth * volume;
					sfxBar->position.x = 960.f - (maxWidth - sfxBar->size.x) * 0.5f;
				}
			}
		}
	}
}