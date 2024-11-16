/* Start Header ************************************************************************/
/*!
\file       MouseEvent.h
\author     Hurng Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu
\date       Sept 23, 2024
\brief      This file contains the declaration of the Mouse Events.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include "Event.h"


namespace Ukemochi {
	/*!
	\brief Event for mouse movement.

	This event is triggered when the mouse is moved.
	*/
	class UME_API MouseMovedEvent : public Event
	{
	public:
		/*!
		\brief Constructor for MouseMovedEvent.
		\param x The x-coordinate of the mouse position.
		\param y The y-coordinate of the mouse position.
		*/
		MouseMovedEvent(float x, float y)
			: m_MouseX(x), m_MouseY(y) {}

		/*!
	   \brief Retrieves the x-coordinate of the mouse position.
	   \return The x-coordinate.
	   */
		inline float GetX() const { return m_MouseX; }
		/*!
	   \brief Retrieves the y-coordinate of the mouse position.
	   \return The y-coordinate.
	   */
		inline float GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved) //!< Event type for mouse moved.
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput) //!< Event categories.

	private:
		float m_MouseX; //!< The x-coordinate of the mouse position.
		float m_MouseY; //!< The y-coordinate of the mouse position.
	};

	/*!
	\brief Event for mouse scrolling.

	This event is triggered when the mouse is scrolled.
	*/
	class UME_API MouseScrolledEvent : public Event
	{
	public:
		/*!
		\brief Constructor for MouseScrolledEvent.
		\param xOffset The x offset for the scroll.
		\param yOffset The y offset for the scroll.
		*/
		MouseScrolledEvent(float xOffset, float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) {}

		/*!
		\brief Retrieves the x offset of the scroll.
		\return The x offset.
		*/
		inline float GetXOffset() const { return m_XOffset; }
		/*!
		\brief Retrieves the y offset of the scroll.
		\return The y offset.
		*/
		inline float GetYOffset() const { return m_YOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float m_XOffset, m_YOffset; // The x and y offset of the scroll.
	};

	/*!
	\brief Base class for mouse button events.

	This class contains the common functionality for mouse button events.
	*/
	class UME_API MouseButtonEvent : public Event
	{
	public:
		/*!
		\brief Retrieves the mouse button associated with the event.
		\return The mouse button.
		*/
		inline int GetMouseButton() const { return m_Button; }
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)


	protected:
		/*!
		\brief Constructor for MouseButtonEvent.
		\param button The mouse button associated with the event.
		*/
		MouseButtonEvent(int button)
			: m_Button(button) {}

		int m_Button; //!< The mouse button.
	}; 

	/*!
	\brief Event for mouse button pressed.

	This event is triggered when a mouse button is pressed.
	*/
	class UME_API MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		/*!
		\brief Constructor for MouseButtonPressedEvent.
		\param button The mouse button that was pressed.
		*/
		MouseButtonPressedEvent(int button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	/*!
	\brief Event for mouse button released.

	This event is triggered when a mouse button is released.
	*/
	class UME_API MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button)
			: MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}

