#pragma once

#include "crpch.h"

#include "core/core.h"
#include "core/Events/Event.h"

namespace CR {

	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "Cosy Raccoon Engine",
			unsigned int width = 1200,
			unsigned int height = 720)
			: Title(title), Width(width), Height(height)
		{

		}
	};

	class CR_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;


		//Windows attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVsync(bool enabled) = 0;
		virtual bool IsVsync() const = 0;

		static Window* Create(const WindowProps& props = WindowProps()); // Get implement in Platform

	};
}