/* Start Header ************************************************************************/
/*!
\file       EntryPoint.h
\author     Hurng Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu
\date       Sept 15, 2024
\brief      This header file defines the main entry point for the Ukemochi Engine application.
            It initializes logging, sets up memory leak detection, and runs the application loop.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include <crtdbg.h>				// To check for memory leaks

#ifdef UME_PLATFORM_WINDOWS

extern Ukemochi::Application* Ukemochi::CreateApplication();

#ifdef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:CONSOLE")
int main()
#endif // _DEBUG

#ifndef _DEBUG
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif // !_DEBUG
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Initialize logging
	Ukemochi::Log::Init();
	UME_ENGINE_WARN("Inititalized Log");

	// Create and run the application
	auto app = Ukemochi::CreateApplication();
	app->GameLoop();
	// Cleanup
	delete app;
}

#endif
