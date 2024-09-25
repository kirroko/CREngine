#include "PreCompile.h"
#include "Application.h"
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

		while (m_running)
		{
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

			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
			if (Input::IsKeyPressed(GLFW_KEY_W))
			{
				// If 'W' key is pressed, move forward
				UME_ENGINE_INFO("W key is pressed");
			}

			//test comment

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
		}
	}
}