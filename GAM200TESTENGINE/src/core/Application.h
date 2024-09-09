#pragma once
#include "core.h"
#include "Events/Event.h"
#include "Window.h"

namespace CR {


	class CR_API Application
	{

	public:
		Application();
		virtual ~Application();
		void run();

	private:
		std::unique_ptr<Window> m_Window;
		bool m_running = true;
	};

	// To be defined in CLIENT (GAME)
	Application* CreateApplication();
}
