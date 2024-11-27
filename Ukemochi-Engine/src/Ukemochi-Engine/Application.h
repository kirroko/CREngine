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
		Application();
		virtual ~Application();
		void GameLoop();
		void UpdateFPS();
		void DrawFPS();
		void UpdateIMGUI();
		void StartGame();
		void StopGame();
		void EventIsOn(Event& e);

		static Application& Get() { return *s_Instance; }
		WindowsWindow& GetWindow() { return *m_Window; } // Access the current window

	private:

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

