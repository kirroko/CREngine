#pragma once

#include "PreCompile.h"
#include "Ukemochi-Engine/Core.h"
#include "Ukemochi-Engine/Events/Event.h"
#include <../vendor/GLFW/include/GLFW/glfw3.h>

namespace UME {

	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "Cozy Raccoon Engine",
			unsigned int width = 1920,
			unsigned int height = 1080)
			: Title(title), Width(width), Height(height)
		{
		}
	};


	class WindowsWindow
	{
	public:
		WindowsWindow(const WindowProps& props);
		~WindowsWindow();

		void OnUpdate();

		unsigned int GetWidth() const { return m_Data.Width; }
		unsigned int GetHeight() const { return m_Data.Height; }

		void SetEventCallback(const std::function<void(Event&)>& callback) { m_Data.EventCallback = callback; }
		void SetVsync(bool enabled);
		bool IsVsync() const { return m_Data.VSync; }

		void* GetNativeWindow() const { return m_Window; }

	private:
		void Init(const WindowProps& props);
		void Shutdown();

		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			std::function<void(Event&)> EventCallback;
		};

		WindowData m_Data;
		static bool s_GLFWInitialized;
	};

} // namespace UME
