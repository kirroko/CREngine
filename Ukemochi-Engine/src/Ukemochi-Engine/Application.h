/* Start Header ************************************************************************/
/*!
\file       Application.h
\author     HURNG Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu (50%)
\co-authors TAN Si Han, t.sihan , 2301264, t.sihan\@digipen.edu (30%)
\co-authors Wong Jun Yu Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu (15%)
\co-authors Pek Jun Kai Gerald, p.junkaigerald, 2301334, p.junkaigerald\@digipen.edu (5%)
\date       Sept 25, 2024
\brief      This file contains the function declarations of application.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include "Core.h"
#include "FileWatcher.h"
#include "../Platform/WindowsWindow.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "ImGui/ImGuiCore.h"


#pragma warning(push)
#pragma warning(disable : 4251)

namespace Ukemochi {


	class UME_API Application
	{

	public:
		/*!***********************************************************************
		\brief
		 Initializes the application, setting up the window, ImGui, and file watcher.
		*************************************************************************/
		Application();
		/*!***********************************************************************
		\brief
		 Cleans up resources used by the application, such as the window, ImGui,
		 and file watcher.
		\return
		 None.
		*************************************************************************/
		virtual ~Application();
		/*!***********************************************************************
		\brief
		 The main application loop that handles updates, rendering, and events.
		\return
		 None.
		*************************************************************************/
		void GameLoop();
		/*!***********************************************************************
		\brief
		 Updates the FPS counter by calculating the frame time and updating the display.
		\return
		 None.
		*************************************************************************/
		void UpdateFPS();

		/*!***********************************************************************
		\brief
		 Draws the current FPS counter on the screen.
		\return
		 None.
		*************************************************************************/
		void DrawFPS();
		/*!***********************************************************************
		\brief
		 Updates the ImGui interface, including rendering UI elements and
		 processing input events.
		\return
		 None.
		*************************************************************************/
		void UpdateIMGUI();
		/*!***********************************************************************
		\brief
		 Starts the game by initializing necessary resources and setting the
		 game state to active.
		\return
		 None.
		*************************************************************************/
		void StartGame();
		/*!***********************************************************************
		\brief
		 Stops the game, releasing resources and resetting the game state to inactive.
		\return
		 None.
		*************************************************************************/
		void StopGame();
		/*!***********************************************************************
		\brief
		 Processes events received by the application.
		\param[in] e
		 The event to be processed.
		\return
		 None.
		*************************************************************************/
		void EventIsOn(Event& e);

		/*!***********************************************************************
		\brief
		 Retrieves the singleton instance of the application.
		\return
		 A reference to the application instance.
		*************************************************************************/
		static Application& Get() { return *s_Instance; }
		/*!***********************************************************************
		\brief
		 Retrieves the current application window.
		\return
		 A reference to the current WindowsWindow object.
		*************************************************************************/
		WindowsWindow& GetWindow() { return *m_Window; } // Access the current window

		bool GameStarted = false; // Has the game started?
		bool IsPaused = false; // Is the game paused?

		bool Paused() const { return pause; }
		void SetPaused(bool paused) { pause = paused; }

		void QuitGame();

	private:
		bool pause = false;
		/*!***********************************************************************
		\brief
		 Checks if a window close event has been triggered.
		\param[in] e
		 The window close event to evaluate.
		\return
		 True if the window is marked for closing; false otherwise.
		*************************************************************************/
		bool IsWindowClose(WindowCloseEvent& e);
		std::unique_ptr<WindowsWindow> m_Window;
		std::atomic_bool m_running = true; // Atomic flag for thread control
		std::atomic_bool m_scriptChanged = false;

		static Application* s_Instance;

		UseImGui imguiInstance;

		std::shared_ptr<Ukemochi::FileWatcher> fwInstance;

		bool m_docking_enabled = true;
	};

	// To be defined in CLIENT (GAME)
	Application* CreateApplication();
}
#pragma warning(pop)

