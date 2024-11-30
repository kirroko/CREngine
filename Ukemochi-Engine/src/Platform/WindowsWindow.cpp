/* Start Header ************************************************************************/
/*!
\file       WindowsWindow.cpp
\author     Hurng Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu (85%)
\co-authors Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu (15%)
\date       Nov 30, 2024
\brief      This file contains the declaration and implementation of the WindowsWindow class, 
            which manages window creation, input handling, and rendering for a windowed application using GLFW.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "WindowsWindow.h"
#include "Ukemochi-Engine/Events/ApplicationEvent.h"
#include "Ukemochi-Engine/Events/KeyEvent.h"
#include "Ukemochi-Engine/Events/MouseEvent.h"
#include "Ukemochi-Engine/Input/Input.h"
#include "Ukemochi-Engine/Application.h"
#include "Ukemochi-Engine/Audio/Audio.h"
#include <glad/glad.h>

namespace Ukemochi {

	// Static variable to track the GLFW initialization status.
	bool WindowsWindow::s_GLFWInitialized = false;

	/*!
	\brief GLFW error callback function to handle error messages.
	\param error_code The error code.
	\param description The error description.
	*/
	static void GLFWErrorCB(int error_code, const char* description)
	{
		UME_ENGINE_ERROR("GLFW Error ({0}): {1}", error_code, description);
	}

	/*!
	\brief Constructs a WindowsWindow instance and initializes it.
	\param props The properties for the window.
	*/
	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}
	/*!
	\brief Destroys the WindowsWindow instance and cleans up resources.
	*/
	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}
	/*!
	\brief Initializes the window with the specified properties.
	\param props The properties for window creation.
	*/
	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

#ifdef _DEBUG
		m_Data.IsFullScreen = false;
#endif // _DEBUG

#ifndef _DEBUG
		m_Data.IsFullScreen = true;
#endif // !_DEBUG

		m_Data.PrevWidth = props.Width;
		m_Data.PrevHeight = props.Height;

		UME_ENGINE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		// Initialize GLFW if not already done
		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			UME_ENGINE_ASSERT(success, "Could not initialize GLFW");
			glfwSetErrorCallback(GLFWErrorCB);
			s_GLFWInitialized = true;
			(void)success;  // Mark the variable as used to suppress the warning
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef _DEBUG
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Disable window resizing
#endif // !_DEBUG

		// Create GLFW window
		if (m_Data.IsFullScreen)
			m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), glfwGetPrimaryMonitor(), NULL);
		else
			m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);

		// Load OpenGL function pointers
		gladLoadGL();
		//int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		//UME_ENGINE_ASSERT(status, "Failed to initialize GLAD");
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVsync(true);

		glfwSetDropCallback(m_Window, fileDropCallback);

		glViewport(0, 0, props.Width, props.Height);

		// GLFW CALLBACK // Set GLFW callbacks for various events
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

				(void)scancode; // Suppress the warning by marking scancode as unused

				(void)mods;

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

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int character)
			{
				WindowData& info = *(WindowData*)(glfwGetWindowUserPointer(window));
				KeyTypedEvent event(character);
				info.EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& info = *(WindowData*)(glfwGetWindowUserPointer(window));

				(void)mods;
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

		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow*, int focused)
			{
				if (focused) // Window gain focus
				{
					Application::Get().IsPaused = false;
#ifndef _DEBUG
					Audio::GetInstance().PlayGameBGM();
#endif // !_DEBUG
				}
				else // Window lost focus
				{
					Application::Get().IsPaused = true;
					Audio::GetInstance().StopAllSound();
				}
			});

		glfwSetWindowIconifyCallback(m_Window, [](GLFWwindow*, int iconified)
			{
				if (iconified) // Window is minimized
				{
					Application::Get().IsPaused = true;
					Audio::GetInstance().StopAllSound();
				}
				else // Window is restored
				{
					Application::Get().IsPaused = false;
#ifndef _DEBUG
					Audio::GetInstance().PlayGameBGM();
#endif // !_DEBUG
				}
			});
	}
	/*!
	\brief Shuts down the window and releases resources.
	*/
	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}
	/*!
	\brief Toggle the screen between fullscreen and window modes.
	*/
	void WindowsWindow::ToggleFullscreen()
	{
		// Press F11 to toggle screen modes
		if (Input::IsKeyTriggered(UME_KEY_F11))
		{
			// Switch to fullscreen mode
			if (!m_Data.IsFullScreen)
			{
				// Save the current window position and size
				glfwGetWindowPos(m_Window, &m_Data.PrevX, &m_Data.PrevY);
				glfwGetWindowSize(m_Window, &m_Data.PrevWidth, &m_Data.PrevHeight);

				// Get the primary monitor
				GLFWmonitor* monitor = glfwGetPrimaryMonitor();
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);

				glfwSetWindowMonitor(m_Window, monitor, 0, 0, m_Data.Width, m_Data.Height, mode->refreshRate);
				m_Data.IsFullScreen = true;
			}
			else // Switch to windowed mode
			{
				glfwSetWindowMonitor(m_Window, nullptr, m_Data.PrevX, m_Data.PrevY, m_Data.PrevWidth, m_Data.PrevHeight, 0);
				m_Data.IsFullScreen = false;
			}
		}
	}

	/*!
	\brief Updates the window by polling events and swapping buffers.
	*/
	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
		
		ToggleFullscreen();
	}
	/*!
	\brief Sets vertical synchronization for the window.
	\param enabled A boolean indicating whether VSync should be enabled or disabled.
	*/
	void WindowsWindow::SetVsync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	//bool fileDropped = false;
	void WindowsWindow::fileDropCallback(GLFWwindow* window, int count, const char** paths)
	{
		if (count > 0)
		{
			std::cout << "File dropped: " << paths[0] << std::endl;
			WindowsWindow* win = (WindowsWindow*)glfwGetWindowUserPointer(window);
			win->m_FilePath = std::string(paths[0]);
			win->fileDropped = true; // Set flag when a file is dropped
		}
	}

	//bool WindowsWindow::IsVsync() const
	//{
	//	return m_Data.VSync;
	//}

} // namespace UME
