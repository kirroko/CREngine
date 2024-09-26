#include "PreCompile.h"
#include "ImGuiCore.h"

#include "imgui.h"
#include "Platform/ImGuiOpenGl/ImGuiOpenGl.h"
#include "GLFW/glfw3.h"
#include "../Application.h"

namespace UME
{
	float UseImGui::m_Time = 0.0f;

	void UseImGui::ImGuiInit(GLFWwindow* window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsClassic();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void UseImGui::NewFrame()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		float time = (float)glfwGetTime();
		io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
		m_Time = time;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();
		static bool show = true;
		ImGui::ShowDemoWindow(&show);
		/*ImGui::Begin("Sample Window");
		ImGui::Text("Hello, world!");
		ImGui::End();*/
	}

	void UseImGui::ImGuiUpdate()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void UseImGui::ImGuiClean()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
	}

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