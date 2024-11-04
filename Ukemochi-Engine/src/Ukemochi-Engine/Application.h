#pragma once
#include "Core.h"
#include "../Platform/WindowsWindow.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "ImGui/ImGuiCore.h"


#pragma warning(push)
#pragma warning(disable : 4251)

namespace Ukemochi {


	class UME_API Application
	{

	public:
		Application();
		virtual ~Application();
		void GameLoop();
		void UpdateFPS();
		void DrawFPS();
		void UpdateIMGUI();

		void EventIsOn(Event& e);

		static Application& Get() { return *s_Instance; }
		WindowsWindow& GetWindow() { return *m_Window; } // Access the current window

	private:

		bool IsWindowClose(WindowCloseEvent& e);
		std::unique_ptr<WindowsWindow> m_Window;
		bool m_running = true;

		static Application* s_Instance;

		UseImGui imguiInstance;

		bool m_docking_enabled = true;
	};

	// To be defined in CLIENT (GAME)
	Application* CreateApplication();
}
#pragma warning(pop)

