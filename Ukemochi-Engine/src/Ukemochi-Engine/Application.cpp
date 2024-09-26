#include "PreCompile.h"
#include "Application.h"
#include "Game/GSM.h"
#include "Ukemochi-Engine/Logs/Log.h"
#include "FrameController.h"
#include <iomanip>
#include <Ukemochi-Engine/Input.h>
#include <glad/glad.h>

namespace UME {
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		s_Instance = this;
		WindowProps props; // You can customize these properties if needed
		m_Window = std::make_unique<WindowsWindow>(props);
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::EventIsOn));
	}

	Application::~Application()
	{
	}

	void Application::EventIsOn(Event& e)
	{
		EventDispatcher dispatch(e);
		dispatch.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::IsWindowClose));

		//UME_ENGINE_TRACE("{0}", e.ToString());
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

		//Init GameStateManager
		Ukemochi::GSM_Initialize(Ukemochi::GS_STATES::GS_MAINMENU);

		while (Ukemochi::gsm_current != Ukemochi::GS_STATES::GS_QUIT && m_running)
		{
			//current state != restart
			if (Ukemochi::gsm_current != Ukemochi::GS_STATES::GS_RESTART)
			{
				Ukemochi::GSM_Update();
				Ukemochi::gsm_fpLoad();
			}
			else
			{
				Ukemochi::gsm_next = Ukemochi::gsm_current = Ukemochi::gsm_previous;
			}

			//Init Scene
			Ukemochi::gsm_fpInitialize();
			
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
			if (Input::IsKeyPressed(GLFW_KEY_W))
			{
				// If 'W' key is pressed, move forward
				UME_ENGINE_INFO("W key is pressed");
			}

			//Current Scene
			while (Ukemochi::gsm_current == Ukemochi::gsm_next && m_running)
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

				//************ SCENE ************
				glClearColor(1, 0, 1, 1);
				glClear(GL_COLOR_BUFFER_BIT);
				m_Window->OnUpdate();

				//Update and Draw current scene
				Ukemochi::gsm_fpUpdate();
				Ukemochi::gsm_fpDraw();

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
			Ukemochi::gsm_fpFree();

			//If game not restart unload Scene
			if (Ukemochi::gsm_next != Ukemochi::GS_STATES::GS_RESTART)
			{
				Ukemochi::gsm_fpUnload();
			}

			Ukemochi::gsm_previous = Ukemochi::gsm_current = Ukemochi::gsm_next;
		}


		//while (m_running)
		//{
		//	g_FrameRateController.Update();

		//	// Use deltaTime from the FrameRateController
		//	double deltaTime = g_FrameRateController.GetDeltaTime();
		//	accumulator += deltaTime;

		//	// Run game logic at a fixed time step (e.g., 60 times per second)
		//	while (accumulator >= fixedTimeStep)
		//	{
		//		// Update game logic with a fixed delta time
		//		accumulator -= fixedTimeStep;
		//	}

		//	glClearColor(1, 0, 1, 1);
		//	glClear(GL_COLOR_BUFFER_BIT);
		//	m_Window->OnUpdate();

		//	double currentTime = glfwGetTime();
		//	// Only log/display the FPS every second (or defined interval)
		//	if (currentTime - lastFPSDisplayTime >= fpsDisplayInterval)
		//	{
		//		double fps = g_FrameRateController.GetFPS();

		//		// Use std::ostringstream to format the FPS with 2 decimal places
		//		std::ostringstream oss;
		//		oss << std::fixed << std::setprecision(2) << fps;
		//		std::string fpsString = oss.str();

		//		// Log or display the FPS
		//		UME_ENGINE_INFO("FPS: {0}", fpsString);

		//		// Update the last time we displayed the FPS
		//		lastFPSDisplayTime = currentTime;
		//	}
		//}
	}
}