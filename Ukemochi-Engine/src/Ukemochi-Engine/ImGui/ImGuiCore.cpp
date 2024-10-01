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
#include "Platform/ImGuiOpenGl/ImGuiOpenGl.h"
#include "GLFW/glfw3.h"
#include "../Application.h"

namespace UME
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
		ImGui::StyleColorsClassic();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		ImGui_ImplOpenGL3_Init("#version 410");
	}
	/*!
	\brief Prepares a new ImGui frame and handles window dimensions and timing.
	*/
	void UseImGui::NewFrame()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(static_cast<float>(app.GetWindow().GetWidth()), static_cast<float>(app.GetWindow().GetHeight()));

		float time = (float)glfwGetTime();
		io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
		m_Time = time;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();
		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		static bool showAnotherWindow = true;
		if (showAnotherWindow)
		{
			ImGui::Begin("Another Window", &showAnotherWindow);   // Create a window called "Another Window"
			ImGui::Text("This is another window!");               // Display some text
			if (ImGui::Button("Close Me"))                        // Close button logic
			{
				WindowCloseEvent event;
				app.EventIsOn(event);
			}
			ImGui::End();
		}
		/*ImGui::Begin("Sample Window");
		ImGui::Text("Hello, world!");
		ImGui::End();*/
	}

	/*!
	\brief Renders the current ImGui frame and draws the UI.
	*/
	void UseImGui::ImGuiUpdate()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	/*!
	\brief Cleans up ImGui resources.
	*/
	void UseImGui::ImGuiClean()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
	}
	/*!
	\brief Dispatches events to ImGui for handling input.
	\param event Reference to the event to be dispatched.
	*/
	void UseImGui::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseButtonPressedEvent>(UME_BIND_EVENT(UseImGui::IsMouseButtonPressedEvent));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(UME_BIND_EVENT(UseImGui::IsMouseButtonReleasedEvent));
		dispatcher.Dispatch<MouseMovedEvent>(UME_BIND_EVENT(UseImGui::IsMouseMovedEvent));
		dispatcher.Dispatch<MouseScrolledEvent>(UME_BIND_EVENT(UseImGui::IsMouseScrolledEvent));
		dispatcher.Dispatch<KeyPressedEvent>(UME_BIND_EVENT(UseImGui::IsKeyPressedEvent));
		dispatcher.Dispatch<KeyReleasedEvent>(UME_BIND_EVENT(UseImGui::IsKeyReleasedEvent));
		dispatcher.Dispatch<KeyTypedEvent>(UME_BIND_EVENT(UseImGui::IsKeyTypeEvent));
		dispatcher.Dispatch<WindowResizeEvent>(UME_BIND_EVENT(UseImGui::IsWindowResizeEvent));
	}

	// Mouse and keyboard event handling
	bool UseImGui::IsMouseButtonPressedEvent(MouseButtonPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.GetMouseButton()] = true;
		
		return false;
	}

	bool UseImGui::IsMouseButtonReleasedEvent(MouseButtonReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.GetMouseButton()] = false;

		return false;
	}

	bool UseImGui::IsMouseMovedEvent(MouseMovedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.GetX(), e.GetY());

		return false;
	}

	bool UseImGui::IsMouseScrolledEvent(MouseScrolledEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += e.GetXOffset();
		io.MouseWheel += e.GetYOffset();

		return false;
	}

	bool UseImGui::IsKeyPressedEvent(KeyPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.GetKeyCode()] = true;

		return false;
	}

	bool UseImGui::IsKeyReleasedEvent(KeyReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.GetKeyCode()] = false;
		return false;
	}

	bool UseImGui::IsWindowResizeEvent(WindowResizeEvent& e)
	{
		(void)e;// Suppress the unused parameter warning
		return false;
	}

	bool UseImGui::IsKeyTypeEvent(KeyTypedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		int character = e.GetKeyCode();
		if (character > 0 && character < 0x10000)
			io.AddInputCharacter(character);

		return false;
	}
}