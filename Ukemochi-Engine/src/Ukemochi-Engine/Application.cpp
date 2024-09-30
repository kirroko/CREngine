/* Start Header ************************************************************************/
/*!
\file       Application.cpp
\par		Ukemochi
\author 	HURNG Kai Rui, h.kairui, 230xxxx, h.kairui\@digipen.edu (50%)
\co-authors x, x, 230xxxx, x\@digipen.edu (x%)
			Wong Jun Yu Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu (x%)
			x, x, 230xxxx, x\@digipen.edu (x%)
			x, x, 230xxxx, x\@digipen.edu (x%)
\par        Course: CSD2400/CSD2401
\date   	25-09-2024
\brief      This file contains the function definitions of application.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#include "PreCompile.h"
#include "Application.h"
#include "Game/GSM.h"
#include "Ukemochi-Engine/Logs/Log.h"
#include "FrameController.h"

// System Includes
#include "Input/Input.h" // for input system
#include "Physics/Physics.h"	   // for physics system
#include "Collision/Collision.h"   // for collision system
#include "Graphics/Renderer.h"     // for graphics system

#include "ECS/ECS.h"
#include <iomanip>

//#include <glad/glad.h>
#include "Audio/Audio.h"
#include "ImGui/ImGuiCore.h"

using namespace Ukemochi;

namespace UME {
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		s_Instance = this;
		WindowProps props; // You can customize these properties if needed
		m_Window = std::make_unique<WindowsWindow>(props);
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::EventIsOn));

		// Set up ECS
		ECS::GetInstance().Init();

		// Register your components
		ECS::GetInstance().RegisterComponent<Transform>();
		ECS::GetInstance().RegisterComponent<Rigidbody2D>();
		ECS::GetInstance().RegisterComponent<BoxCollider2D>();
		ECS::GetInstance().RegisterComponent<CircleCollider2D>();
		ECS::GetInstance().RegisterComponent<SpriteRender>();

		// Register your systems
		ECS::GetInstance().RegisterSystem<Physics>();
		ECS::GetInstance().RegisterSystem<Collision>();
		ECS::GetInstance().RegisterSystem<Renderer>();

		// Set a signature to your system
		// Each system will have a signature to determine which entities it will process

		// For physics system
		SignatureID sig;
		sig.set(ECS::GetInstance().GetComponentType<Transform>());
		sig.set(ECS::GetInstance().GetComponentType<Rigidbody2D>());
		ECS::GetInstance().SetSystemSignature<Physics>(sig);

		// For renderer system
		sig.reset();
		sig.set(ECS::GetInstance().GetComponentType<Transform>());
		sig.set(ECS::GetInstance().GetComponentType<SpriteRender>());
		ECS::GetInstance().SetSystemSignature<Renderer>(sig);

		// For collision system
		sig.reset();
		sig.set(ECS::GetInstance().GetComponentType<Transform>());
		sig.set(ECS::GetInstance().GetComponentType<BoxCollider2D>());
		ECS::GetInstance().SetSystemSignature<Collision>(sig);

		GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(m_Window->GetNativeWindow());
		imguiInstance.ImGuiInit(glfwWindow);
	}

	Application::~Application()
	{
		imguiInstance.ImGuiClean();
	}

	void Application::EventIsOn(Event& e)
	{
		imguiInstance.OnEvent(e);
		EventDispatcher dispatch(e);
		dispatch.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::IsWindowClose));
		UME_ENGINE_TRACE("{0}", e.ToString());
	}

	bool Application::IsWindowClose(WindowCloseEvent& e)
	{
		m_running = false;
		return true;
	}

	void Application::GameLoop()
	{
		double accumulator = 0.0;
		const double fixedTimeStep = 1.0 / 60.0; // Fixed timestep for game logic

		double lastFPSDisplayTime = 0.0; // To track when we last displayed the FPS
		double fpsDisplayInterval = 1.0; // Display the FPS every 1 second

		//render.drawBox(800.f, 450.f, 1600.f, 900.f, "../Assets/Textures/Moon Floor.png");
		//render.drawCircle(800.f, 450.f, 500.f, "../Assets/Textures/container.jpg");

		Audio audio;
		audio.CreateGroup("test");
		audio.LoadSound(R"(C:\Users\tansi\OneDrive\Desktop\BGM_game.mp3)");
		audio.PlaySoundInGroup(AudioList::BGM, ChannelGroups::MENUAUDIO);

		
		//render.drawCircle(800.f, 450.f, 100.f, "../Assets/Textures/container.jpg");
		
		//render.drawBox(800.f, 450.f, 100.f, 100.f, "");


		bool isToggling = false;
		while (m_running)
		{
			g_FrameRateController.Update();
			//Init GameStateManager
			GSM_Initialize(GS_STATES::GS_LEVEL1);

			while (gsm_current != GS_STATES::GS_QUIT && m_running)
			{
				//current state != restart
				if (gsm_current != GS_STATES::GS_RESTART)
				{
					GSM_Update();
					gsm_fpLoad();
				}
				else
				{
					gsm_next = gsm_current = gsm_previous;
				}

			gsm_fpInitialize();

			// Get the current time
			double Time = glfwGetTime();
			
			// Check for 'S' key press and toggle scaling
			if (Input::IsKeyPressed(GLFW_KEY_S))
			{
				if (!isToggling)
				{
					render.ToggleInputsForScale();
					isToggling = true;
				}
			}
			else if (Input::IsKeyPressed(GLFW_KEY_R))
			{
				if (!isToggling)
				{
					render.ToggleInputsForRotation();
					isToggling = true;
				}
			}
			else if (Input::IsKeyPressed(GLFW_KEY_D))
			{
				if (!isToggling)
				{
					render.debug_mode_enabled = !render.debug_mode_enabled;
					isToggling = true;
				}
			}
			else
			{
				isToggling = false;
			}


			render.render();
			m_Window->OnUpdate();

			if (Input::IsKeyPressed(GLFW_KEY_W))
			{
				// If 'W' key is pressed, move forward
				UME_ENGINE_INFO("W key is pressed");
			}

				//Current Scene
				while (gsm_current == gsm_next && m_running)
				{
					//************ FPS ************
					g_FrameRateController.Update();

					// Use deltaTime from the FrameRateController
					double deltaTime = g_FrameRateController.GetDeltaTime();
					accumulator += deltaTime;

					// Run game logic at a fixed time step (e.g., 60 times per second)
					while (accumulator >= fixedTimeStep)
					{
						// Update game logic with a fixed delta time
						accumulator -= fixedTimeStep;
					}
					//************ FPS ************

					/*glClearColor(1, 0, 1, 1);
					glClear(GL_COLOR_BUFFER_BIT);
					m_Window->OnUpdate();*/

					//// Add a box and a circle
					//render.addObjects(GameObject(100.0f, 200.0f, 50.0f, 50.0f, true));  // Box
					//render.addObjects(GameObject(0.0f, 0.0f, 1600.0f, 900.0f, true));  // Box
					//render.addObjects(GameObject(300.0f, 400.0f, 75.0f, false));        // Circle

			//render.drawBox(0, 0, 100, 100, true);
			//render.render();



			if (Input::IsKeyPressed(GLFW_KEY_W))
			{
				// If 'W' key is pressed, move forward
				UME_ENGINE_INFO("W key is pressed");
			}
					//render.drawBox(0, 0, 100, 100, true);
					//render->drawCircle(800.f, 450.f, 500.f, "../Assets/Textures/container.jpg");
					//render->render();
					
					imguiInstance.NewFrame();


					imguiInstance.ImGuiUpdate(); // Render ImGui elements
					m_Window->OnUpdate();
					if (Input::IsKeyPressed(GLFW_KEY_W))
					{
						// If 'W' key is pressed, move forward
						UME_ENGINE_INFO("W key is pressed");
					}

					//Update and Draw current scene
					gsm_fpUpdate();
					gsm_fpDraw();

					//************ FPS ************
					double currentTime = glfwGetTime();
					// Only log/display the FPS every second (or defined interval)
					if (currentTime - lastFPSDisplayTime >= fpsDisplayInterval)
					{
						double fps = g_FrameRateController.GetFPS();

						// Use std::ostringstream to format the FPS with 2 decimal places
						std::ostringstream oss;
						oss << std::fixed << std::setprecision(2) << fps;
						std::string fpsString = oss.str();

						// Log or display the FPS
						UME_ENGINE_INFO("FPS: {0}", fpsString);

						// Update the last time we displayed the FPS
						lastFPSDisplayTime = currentTime;
					}
					//************ FPS ************
				}

				//Free scene
				gsm_fpFree();

				//If game not restart unload Scene
				if (gsm_next != GS_STATES::GS_RESTART)
				{
					gsm_fpUnload();
				}

				gsm_previous = gsm_current = gsm_next;
			}
		}
	}
}