#include "UMEpch.h"
#include "Application.h"
#include "Ukemochi-Engine/Log.h"

#include <GLFW/glfw3.h>

namespace UME {

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::EventIsOn));
	}

	Application::~Application()
	{

	}

	void Application::EventIsOn(Event& e)
	{
		EventDispatcher dispatch(e);
		dispatch.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::IsWindowClose));

		UME_ENGINE_TRACE("{0}", e.ToString());
	}

	bool Application::IsWindowClose(WindowCloseEvent& e)
	{
		m_running = false;
		return true;
	}

	void Application::run()
	{
		while (m_running)
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
			//float fps = m_Window->GetFPS();
			//UME_ENGINE_INFO("FPS: {0}", fps);
		}
	}

}
