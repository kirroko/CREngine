#include "PreCompile.h"
#include "Application.h"
#include "Ukemochi-Engine/Logs/Log.h"
#include "FrameController.h"
#include <iomanip>
#include <Ukemochi-Engine/Input.h>
//#include <glad/glad.h>
#include "Ukemochi-Engine/Graphics/Renderer.h"

Renderer render;
namespace UME {
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		s_Instance = this;
		WindowProps props; // You can customize these properties if needed
		m_Window = std::make_unique<WindowsWindow>(props);
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::EventIsOn));
		render.init();
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


		render.drawBox(800.f, 450.f, 1600.f, 900.f, "../Assets/Textures/Moon Floor.png");
		//render.drawCircle(800.f, 450.f, 500.f, "../Assets/Textures/container.jpg");
		render.drawBox(800.f, 450.f, 100.f, 100.f);

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

			// Get the current time
			double Time = glfwGetTime();

			// Check for 'S' key press and toggle scaling, using a debounce timer
			if (Input::IsKeyPressed(GLFW_KEY_S))
			{
				render.ToggleInputsForScale();
			}

			if (Input::IsKeyPressed(GLFW_KEY_R))
			{
				render.ToggleInputsForRotation();
			}
		
			render.render();
			m_Window->OnUpdate();

			if (Input::IsKeyPressed(GLFW_KEY_W))
			{
				// If 'W' key is pressed, move forward
				UME_ENGINE_INFO("W key is pressed");
			}

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