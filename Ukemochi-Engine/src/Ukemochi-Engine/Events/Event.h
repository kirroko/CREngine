/* Start Header
*****************************************************************/
/*!
\file       Event.h
\author     Hurng Kai Rui, h.kairui, 2301278
\par        email: h.kairui\@digipen.edu
\date       Sept 23, 2024
\brief      This file contains the declaration of the Events.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include "Ukemochi-Engine/Core.h"
#include "PreCompile.h"

namespace UME {

	/*!
	\brief Enumeration of event types.

	This enum class defines the various types of events that can occur in the application.
	*/
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved, AppTick,
		AppUpdate, AppRender, KeyPressed, KeyReleased, KeyTyped, MouseButtonPressed, MouseButtonReleased,
		MouseMoved, MouseScrolled
	};

	/*!
	\brief Enumeration of event categories.

	This enum defines categories that events can belong to, allowing for easier event filtering.
	BIT is a macro that is defined in core.h
	*/
	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

// Macro to define event class type.
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }
// Macro to define event class category.
#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	/*!
	\brief Base class for all events.

	This class provides the basic structure and interface for all event types.
	*/
	class UME_API Event
	{
		friend class EventDispatcher; //!< Grants EventDispatcher access to private members.
	public:
		/*!
		\brief Retrieves the event type.
		\return The type of the event.
		*/
		virtual EventType GetEventType() const = 0;
		/*!
		\brief Retrieves the name of the event.
		\return A string representing the name of the event.
		*/
		virtual const char* GetName() const = 0;
		/*!
		\brief Retrieves the category flags for the event.
		\return The category flags for the event.
		*/
		virtual int GetCategoryFlags() const = 0;
		/*!
		\brief Converts the event to a string representation.
		\return A string describing the event.
		*/
		virtual std::string ToString() const { return GetName(); }

		/*!
		\brief Checks if the event is in a specified category.
		\param category The category to check against.
		\return True if the event belongs to the specified category, false otherwise.
		*/
		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	protected:
		bool m_handled = false;//!< Indicates if the event has been handled.
	};


	/*!
	\brief Class for dispatching events to appropriate handlers.

	This class facilitates the dispatching of events to the functions that handle them.
	*/
	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		/*!
	   \brief Constructor for EventDispatcher.
	   \param event The event to be dispatched.
	   */
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}

		/*!
		\brief Dispatches the event to the specified function if it matches the event type.
		\param func The function to handle the event.
		\return True if the event was handled, false otherwise.
		*/
		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event& m_Event; //!< The event to dispatch.
	};

	/*!
	\brief Overloads the stream insertion operator for events.

	\param os The output stream.
	\param e The event to output.
	\return The output stream after inserting the event's string representation.
	*/
	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}


}