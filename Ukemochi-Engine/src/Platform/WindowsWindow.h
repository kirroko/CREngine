#pragma once

#include "Ukemochi-Engine/Window.h"

#include <GLFW/glfw3.h>

namespace UME {

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVsync(bool enabled) override;
		bool IsVsync() const override;

		float GetFPS() const { return m_FPS; } // New method to get the current FPS

		void UpdateFPS();

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();

	private:
		GLFWwindow* m_Window;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
		std::chrono::high_resolution_clock::time_point m_LastFrameTime;
		float m_FPS = 0.0f; // Stores the current FPS
	};
}