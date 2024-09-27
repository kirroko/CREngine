#include "PreCompile.h"
#include "Application.h"
#include "Game/GSM.h"
#include "Ukemochi-Engine/Logs/Log.h"
#include "FrameController.h"

// System Includes
#include "Physics/Physics.h" // for physics system
#include "Collision/Collision.h" // for collision system

// Component Includes
#include "Physics/Rigidbody2D.h" // for Rigidbody2D component
#include "Collision/BoxCollider2D.h" // for BoxCollider2D component
#include "Collision/CircleCollider2D.h" // for CircleCollider2D component

#include "ECS/ECS.h"
#include <iomanip>
#include <Ukemochi-Engine/Input.h>
//#include <glad/glad.h>
#include "Ukemochi-Engine/Graphics/Renderer.h"
#include "Audio/Audio.h"
#include "ImGui/ImGuiCore.h"

Renderer render;

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
		render.init();
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
		ECS::GetInstance().Init();

		// Register your components
		ECS::GetInstance().RegisterComponent<Transform>();
		ECS::GetInstance().RegisterComponent<Rigidbody2D>();
		ECS::GetInstance().RegisterComponent<BoxCollider2D>();
		ECS::GetInstance().RegisterComponent<CircleCollider2D>();

		// Register your systems
		ECS::GetInstance().RegisterSystem<Physics>();
		ECS::GetInstance().RegisterSystem<Collision>();

		// Set a signature to your system
		// Each system will have a signature to determine which entities it will process
		SignatureID sig;
		sig.set(ECS::GetInstance().GetComponentType<Transform>());
		sig.set(ECS::GetInstance().GetComponentType<Rigidbody2D>());
		ECS::GetInstance().SetSystemSignature<Physics>(sig);

		//ECS::GetInstance().SetSystemSignature<Collision>(sig);

		// Our entities within the game world.
		std::vector<EntityID> entities(MAX_ENTITIES);

		// Create entities
		for (auto& entity : entities)
		{
			entity = ECS::GetInstance().CreateEntity();
			ECS::GetInstance().AddComponent(entity, Transform());
			ECS::GetInstance().AddComponent(entity, Rigidbody2D());
		}

		double accumulator = 0.0;
		const double fixedTimeStep = 1.0 / 60.0; // Fixed timestep for game logic

		double lastFPSDisplayTime = 0.0; // To track when we last displayed the FPS
		double fpsDisplayInterval = 1.0; // Display the FPS every 1 second

		render.drawBox(800.f, 450.f, 1600.f, 900.f, "../Assets/Textures/Moon Floor.png");
		render.drawCircle(800.f, 450.f, 500.f, "../Assets/Textures/container.jpg");

		Audio audio;
		audio.CreateGroup("test");
		audio.LoadSound(R"(C:\Users\tansi\OneDrive\Desktop\BGM_game.mp3)");
		audio.PlaySoundInGroup(AudioList::BGM, ChannelGroups::MENUAUDIO);

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

				//Init Scene
				gsm_fpInitialize();

				glClearColor(1, 0, 1, 1);
				glClear(GL_COLOR_BUFFER_BIT);
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
					render.render();
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

		render.cleanUp();
	}
}