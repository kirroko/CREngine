#pragma once
#include "Core.h"
#include "Window.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

namespace UME {


	class UME_API Application
	{

	public:
		Application();
		virtual ~Application();
		void GameLoop();

		void EventIsOn(Event& e);

	private:

		bool IsWindowClose(WindowCloseEvent& e);
		std::unique_ptr<Window> m_Window;
		bool m_running = true;
	};

	// To be defined in CLIENT (GAME)
	Application* CreateApplication();
}


