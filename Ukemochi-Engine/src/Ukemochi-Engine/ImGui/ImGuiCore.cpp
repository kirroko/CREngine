/* Start Header
*****************************************************************/
/*!
\file       ImGuiCore.cpp
\author     Hurng Kai Rui, h.kairui, 2301278
\par        email: h.kairui\@digipen.edu
\date       Sept 25, 2024
\brief      This file contains the implementation of the UseImGui class,
            which manages the initialization, rendering, and event handling 
            for ImGui within the engine.


Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "PreCompile.h"
#include "ImGuiCore.h"

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include <GLFW/glfw3.h>
#include "../Application.h"

#include "../Factory/Factory.h"
#include "../ECS/ECS.h"

#include "../Graphics/Renderer.h"
namespace Ukemochi
{
	float UseImGui::m_Time = 0.0f; //!< Time since last frame for calculating DeltaTime.


	/*!
	\brief Initializes the ImGui context and sets up OpenGL.
	\param window Pointer to the GLFW window (unused in this implementation).
	*/
	void UseImGui::ImGuiInit(GLFWwindow* /*window*/)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");


		/*IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsClassic();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		ImGui_ImplOpenGL3_Init("#version 410");*/
	}
	/*!
	\brief Prepares a new ImGui frame and handles window dimensions and timing.
	*/
	void UseImGui::NewFrame()
	{

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		//ImGuiIO& io = ImGui::GetIO();
		//Application& app = Application::Get();
		//io.DisplaySize = ImVec2(static_cast<float>(app.GetWindow().GetWidth()), static_cast<float>(app.GetWindow().GetHeight()));

		//float time = (float)glfwGetTime();
		//io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
		//m_Time = time;

		//ImGui_ImplOpenGL3_NewFrame();
		//ImGui::NewFrame();
		//static bool show = true;
		//ImGui::ShowDemoWindow(&show);

		//static bool showAnotherWindow = true;
		//if (showAnotherWindow)
		//{
		//	ImGui::Begin("Another Window", &showAnotherWindow);   // Create a window called "Another Window"
		//	ImGui::Text("This is another window!");               // Display some text
		//	if (ImGui::Button("Close Me"))                        // Close button logic
		//	{
		//		WindowCloseEvent event;
		//		app.EventIsOn(event);
		//	}
		//	ImGui::End();
		//}
		///*ImGui::Begin("Sample Window");
		//ImGui::Text("Hello, world!");
		//ImGui::End();*/
	}

	void UseImGui::ShowEntityManagementUI()
	{
		static char filePath[256] = "../Assets/Player.json";
		//static std::string player_data; // Path for creating player entity
		static int entityToRemove = -1; // Variable to hold the ID of the entity to remove

		ImGui::Text("Entity Management");

		ImGui::InputText("Player Data File", filePath, IM_ARRAYSIZE(filePath));

		if (ImGui::Button("Create Player Entity"))
		{
			if (filePath[0] != '\0') // Ensure a file path is provided
			{
				GameObject playerObject = GameObjectFactory::CreateObject(filePath);
				// Optional: Store playerObject instance ID if needed
				ImGui::Text("Player entity created from: %s", filePath);
			}
			else
			{
				ImGui::Text("Please enter a valid file path!");
			}
		}

		// Input field to specify which entity to remove
		ImGui::InputInt("Entity ID to Remove", &entityToRemove);
		test

		// Button to remove an entity
		if (ImGui::Button("Remove Entity"))
		{
			if (entityToRemove >= 0)
			{
				ECS::GetInstance().DestroyEntity(entityToRemove);
				entityToRemove = -1; // Reset the input after removal
				ImGui::Text("Entity %d removed.", entityToRemove);
			}
			else
			{
				ImGui::Text("Invalid Entity ID!");
			}
		}
	}

	void UseImGui::Begin()
	{
		static bool show = true;
		ImGui::ShowDemoWindow(&show);
		static bool showAnotherWindow = true;
		Application& app = Application::Get();
		//GLuint texture = renderer.getTextureColorBuffer();
		GLuint texture = ECS::GetInstance().GetSystem<Renderer>()->getTextureColorBuffer();
		if (showAnotherWindow)
		{
			ImGui::Begin("Player Loader", &showAnotherWindow);   // Create a window called "Another Window"
			ImGui::Image((ImTextureID)(intptr_t)texture, ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight()), { 0,1 },{1,0});
			ImGui::End();
		}
	}
	/*!
	\brief Renders the current ImGui frame and draws the UI.
	*/
	void UseImGui::ImGuiUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
		/*ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());*/
	}

	/*!
	\brief Cleans up ImGui resources.
	*/
	void UseImGui::ImGuiClean()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	/*!
	\brief Dispatches events to ImGui for handling input.
	\param event Reference to the event to be dispatched.
	*/
}