/* Start Header
*****************************************************************/
/*!
\file       ImGuiCore.h
\author     Hurng Kai Rui, h.kairui, 2301278
\par        email: h.kairui\@digipen.edu
\date       Sept 25, 2024
\brief      This file declares the UseImGui class, which manages the 
            initialization, rendering, and event handling for ImGui 
            within the Ukemochi Engine.


Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include <../vendor/GLFW/include/GLFW/glfw3.h>
#include "Ukemochi-Engine/Events/ApplicationEvent.h"
#include "Ukemochi-Engine/Events/KeyEvent.h"
#include "Ukemochi-Engine/Events/MouseEvent.h"


namespace Ukemochi
{
	class UME_API UseImGui
	{
	public:
		/*!
		\brief Initializes the ImGui context and OpenGL setup.
		\param window Pointer to the GLFW window.
		*/
		static void ImGuiInit(GLFWwindow* window);

		/*!
		\brief Prepares a new ImGui frame.
		*/
		static void NewFrame();

		static void Begin();
		/*!
		\brief Renders the current ImGui frame.
		*/
		static void ImGuiUpdate();
		//void ImGuiRender();
		/*!
		\brief Cleans up ImGui resources.
		*/
		static void ImGuiClean();

		static void ShowEntityManagementUI();
	private:
		// Mouse and keyboard event handlers
		/*bool IsMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool IsMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool IsMouseMovedEvent(MouseMovedEvent& e);
		bool IsMouseScrolledEvent(MouseScrolledEvent& e);
		bool IsKeyPressedEvent(KeyPressedEvent& e);
		bool IsKeyReleasedEvent(KeyReleasedEvent& e);
		bool IsKeyTypeEvent(KeyTypedEvent& e);
		bool IsWindowResizeEvent(WindowResizeEvent& e);*/

		void LoadPlayer(const std::string& player_data);
		static float m_Time; //!< Time since the last frame for delta time calculation.
	};
}