/* Start Header ************************************************************************/
/*!
\file       Application.h
\par		Ukemochi
\author 	HURNG Kai Rui, h.kairui, 230xxxx, h.kairui\@digipen.edu (80%)
\co-authors x, x, 230xxxx, x\@digipen.edu (x%)
			Wong Jun Yu Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu (5%)
			Pek Jun Kai Gerald, p.junkaigerald, 2301334, p.junkaigerald\@digipen.edu (5%)
			x, x, 230xxxx, x\@digipen.edu (x%)
\par        Course: CSD2400/CSD2401
\date   	25-09-2024
\brief      This file contains the function definitions of application.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
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

namespace UME {


	class UME_API Application
	{

	public:
		Application();
		virtual ~Application();
		void GameLoop();

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
	};

	// To be defined in CLIENT (GAME)
	Application* CreateApplication();
}
#pragma warning(pop)

