#pragma once
#include <crtdbg.h>				// To check for memory leaks

#ifdef UME_PLATFORM_WINDOWS

extern UME::Application* UME::CreateApplication();

int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UME::Log::Init();
	UME_ENGINE_WARN("Inititalized Log");

	auto app = UME::CreateApplication();
	app->GameLoop();
	delete app;
}

#endif
