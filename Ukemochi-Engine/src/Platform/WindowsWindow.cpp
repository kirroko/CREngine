#include "PreCompile.h"
#include "WindowsWindow.h"
#include "Ukemochi-Engine/Events/ApplicationEvent.h"
#include "Ukemochi-Engine/Events/KeyEvent.h"
#include "Ukemochi-Engine/Events/MouseEvent.h"

#include <GLFW/glfw3.h> 

namespace UME {

	bool WindowsWindow::s_GLFWInitialized = false;

	static void GLFWErrorCB(int error_code, const char* description)
	{
		UME_ENGINE_ERROR("GLFW Error ({0}): {1}", error_code, description);
	}


	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		UME_ENGINE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			UME_ENGINE_ASSERT(success, "Could not initialize GLFW");
			glfwSetErrorCallback(GLFWErrorCB);
			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		//int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		//UME_ENGINE_ASSERT(status, "Failed to initialize GLAD");
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVsync(true);

		// GLFW CALLBACK
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& info = *(WindowData*)(glfwGetWindowUserPointer(window));

				info.Width = width;
				info.Height = height;

				WindowResizeEvent event(width, height);

				info.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& info = *(WindowData*)(glfwGetWindowUserPointer(window));
				WindowCloseEvent event;

				info.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& info = *(WindowData*)(glfwGetWindowUserPointer(window));

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					info.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					info.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					info.EventCallback(event);
					break;
				}
				}

			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& info = *(WindowData*)(glfwGetWindowUserPointer(window));

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					info.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					info.EventCallback(event);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				WindowData& info = *(WindowData*)(glfwGetWindowUserPointer(window));

				MouseScrolledEvent event((float)xoffset, (float)yoffset);
				info.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
			{
				WindowData& info = *(WindowData*)(glfwGetWindowUserPointer(window));

				MouseMovedEvent event((float)xpos, (float)ypos);

				info.EventCallback(event);
			});
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWindow::SetVsync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	//bool WindowsWindow::IsVsync() const
	//{
	//	return m_Data.VSync;
	//}

} // namespace UME
