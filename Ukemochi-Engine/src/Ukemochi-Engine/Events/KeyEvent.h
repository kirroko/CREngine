/* Start Header
*****************************************************************/
/*!
\file       KeyEvent.h
\author     Hurng Kai Rui, h.kairui, 2301278
\par        email: h.kairui\@digipen.edu
\date       Sept 23, 2024
\brief      This file contains the declaration of the Key Events.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once

#include "Event.h"


namespace UME {
	/*!
	\brief Base class for all keyboard events.

	This class contains the key code associated with the event.
	*/
	class UME_API KeyEvent : public Event
	{
	public:
		/*!
		\brief Retrieves the key code of the key event.
		\return The key code associated with the event.
		*/
		inline int GetKeyCode() const { return m_KeyCode; }

		/*!
		\brief Categorizes the event.
		*/
		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		/*!
	   \brief Constructor for KeyEvent.
	   \param keycode The key code associated with the event.
	   */
		KeyEvent(int keycode)
			:m_KeyCode(keycode) {}

		int m_KeyCode; //!< The key code associated with the event.
	};

	/*!
	\brief Event for when a key is pressed.

	This class extends the KeyEvent class and includes the repeat count.
	*/
	class UME_API KeyPressedEvent : public KeyEvent
	{
	public:
		/*!
		\brief Constructor for KeyPressedEvent.
		\param keycode The key code associated with the event.
		\param repeatCount The number of times the key is pressed.
		*/
		KeyPressedEvent(int keycode, int repeatCount)
			:KeyEvent(keycode), m_RepeatCount(repeatCount) {}

		/*!
		\brief Retrieves the repeat count of the key press.
		\return The number of times the key has been pressed.
		*/
		inline int GetRepeatCount() const { return m_RepeatCount; }

		/*!
		\brief Converts the event to a string representation.
		\return A string describing the key pressed event.
		*/
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		// Event Type
		EVENT_CLASS_TYPE(KeyPressed)

	private:
		int m_RepeatCount;//!< The number of times the key has been pressed.
	};

	/*!
	\brief Event for when a key is released.

	This class extends the KeyEvent class.
	*/
	class UME_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string ToString()
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};
	/*!
	\brief Constructor for KeyTypedEvent.
	\param keycode The key code associated with the event.
	*/
	class UME_API KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keycode)
			:KeyEvent(keycode), m_RepeatCount{ 0 } {}


		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)

	private:
		int m_RepeatCount;
	};
}
