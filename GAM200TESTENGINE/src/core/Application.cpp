#include "crpch.h"
#include "Application.h"
#include "core/Events/ApplicationEvent.h"
#include "core/Log.h"

#include <GLFW/glfw3.h>

namespace CR {

	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application()
	{

	}

	void Application::run()
	{
		while (m_running)
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		}
	}

}