#pragma once
#include <../vendor/GLFW/include/GLFW/glfw3.h>
#include "Ukemochi-Engine/Events/ApplicationEvent.h"
#include "Ukemochi-Engine/Events/KeyEvent.h"
#include "Ukemochi-Engine/Events/MouseEvent.h"

namespace UME
{
	class UME_API UseImGui
	{
	public:
		static void ImGuiInit(GLFWwindow* window);
		static void NewFrame();
		static void ImGuiUpdate();
		//void ImGuiRender();
		static void ImGuiClean();
		void OnEvent(Event& event);
	private:
		bool IsMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool IsMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool IsMouseMovedEvent(MouseMovedEvent& e);
		bool IsMouseScrolledEvent(MouseScrolledEvent& e);
		bool IsKeyPressedEvent(KeyPressedEvent& e);
		bool IsKeyReleasedEvent(KeyReleasedEvent& e);
		bool IsKeyTypeEvent(KeyTypedEvent& e);
		bool IsWindowResizeEvent(WindowResizeEvent& e);

		static float m_Time;
	};
}