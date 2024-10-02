/* Start Header
*****************************************************************/
/*!
\file       WindowsWindow.h
\author     Hurng Kai Rui, h.kairui, 2301278
\par        email: h.kairui\@digipen.edu
\date       Sept 12, 2024
\brief      This file contains the declaration of the WindowsWindow class, 
            which manages window creation, input handling, and rendering 
            for a windowed application using GLFW.


Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once

#include "PreCompile.h"
#include "Ukemochi-Engine/Core.h"
#include "Ukemochi-Engine/Events/Event.h"
#include <../vendor/GLFW/include/GLFW/glfw3.h>

namespace UME {

	/*!
	\brief Structure for window properties including title, width, and height.
	*/
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		/*!
		\brief Constructor to initialize window properties.
		\param title The title of the window.
		\param width The width of the window.
		\param height The height of the window.
		*/
		WindowProps(std::string& title,
			unsigned int width,
			unsigned int height)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	/*!
	\brief Class for managing a GLFW window including event handling and rendering.
	*/
	class WindowsWindow
	{
	public:
		/*!
		\brief Constructor for the WindowsWindow class.
		\param props The properties for the window.
		*/
		WindowsWindow(const WindowProps& props);
		/*!
		\brief Destructor for the WindowsWindow class.
		*/
		~WindowsWindow();

		/*!
		\brief Updates the window by polling events and swapping buffers.
		*/
		void OnUpdate();

		/*!
		\brief Gets the width of the window.
		\return The width of the window.
		*/
		unsigned int GetWidth() const { return m_Data.Width; }
		/*!
		\brief Gets the height of the window.
		\return The height of the window.
		*/
		unsigned int GetHeight() const { return m_Data.Height; }

		/*!
		\brief Sets the event callback for window events.
		\param callback The callback function to handle events.
		*/
		void SetEventCallback(const std::function<void(Event&)>& callback) { m_Data.EventCallback = callback; }
		/*!
		\brief Sets vertical synchronization for the window.
		\param enabled Boolean indicating whether to enable VSync.
		*/
		void SetVsync(bool enabled);
		/*!
		\brief Checks if vertical synchronization is enabled.
		\return True if VSync is enabled, false otherwise.
		*/
		bool IsVsync() const { return m_Data.VSync; }
		/*!
		\brief Gets a pointer to the native GLFW window.
		\return Pointer to the GLFW window.
		*/
		void* GetNativeWindow() const { return m_Window; }

	private:
		/*!
		\brief Initializes the window with the specified properties.
		\param props The properties for window creation.
		*/
		void Init(const WindowProps& props);
		/*!
		\brief Shuts down the window and releases resources.
		*/
		void Shutdown();

		GLFWwindow* m_Window; //!< Pointer to the GLFW window.

		/*!
		\brief Structure to hold window data including title, dimensions, and event callback.
		*/
		struct WindowData
		{
			std::string Title{0};
			unsigned int Width{0}, Height{0};
			bool VSync{};

			std::function<void(Event&)> EventCallback; //!< Callback for handling events.
		};

		WindowData m_Data; //!< Instance of window data.
		static bool s_GLFWInitialized;  //!< Static variable to track GLFW initialization.
	};

} // namespace UME
