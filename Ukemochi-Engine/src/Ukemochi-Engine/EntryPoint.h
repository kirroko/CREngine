/* Start Header
*****************************************************************/
/*!
\file       EntryPoint.h
\author     
\par        
\date       Sept 15, 2024
\brief      This header file defines the main entry point for the Ukemochi Engine application.
            It initializes logging, sets up memory leak detection, and runs the application loop.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include <crtdbg.h>				// To check for memory leaks

#ifdef UME_PLATFORM_WINDOWS

extern UME::Application* UME::CreateApplication();

int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Initialize logging
	UME::Log::Init();
	UME_ENGINE_WARN("Inititalized Log");

	// Create and run the application
	auto app = UME::CreateApplication();
	app->GameLoop();
	// Cleanup
	delete app;
}

#endif
