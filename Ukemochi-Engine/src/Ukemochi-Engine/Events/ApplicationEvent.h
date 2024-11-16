/* Start Header ************************************************************************/
/*!
\file       ApplicationEvent.h
\author     Hurng Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu
\date       Sept 23, 2024
\brief      This file contains the declaration of the Application/Windows Events.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include "Event.h"

namespace Ukemochi {
	/*!
	\brief Event for when a window is resized.

	This event contains the new width and height of the window.
	*/
	class UME_API WindowResizeEvent : public Event
	{
	public:
		/*!
		\brief Constructor for WindowResizeEvent.
		\param width The new width of the window.
		\param height The new height of the window.
		*/
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {}

		// Retrieves the width of the window, return new width
		inline unsigned int GetWidth() const { return m_Width; }
		// Retrieves the height of the window, return new height
		inline unsigned int GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
		//static EventType GetStaticType() { return EventType::WindowResize; }
		//virtual EventType GetEventType() const override { return GetStaticType(); }
		//virtual const char* GetName() const override { return "WindowResize"; }

		//// Manually expand the EVENT_CLASS_CATEGORY macro:
		//virtual int GetCategoryFlags() const override { return EventCategoryApplication; }

	private:
		unsigned int m_Width, m_Height; //new width and new height
	};

	/*!
	\brief Event for when a window is requested to close.

	This event does not carry any additional data.
	*/
	class UME_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}
		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	/*!
	\brief Event that signals an application tick.

	This event does not carry any additional data.
	*/
	class UME_API AppTickEvent : public Event
	{
	public:
		AppTickEvent() {}

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	/*!
	\brief Event that signals an application update.

	This event does not carry any additional data.
	*/
	class UME_API AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	/*!
	\brief Event that signals an application render.

	This event does not carry any additional data.
	*/
	class UME_API AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender)
			EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

}