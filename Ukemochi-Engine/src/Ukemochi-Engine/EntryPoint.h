#pragma once

#ifdef UME_PLATFORM_WINDOWS

extern UME::Application* UME::CreateApplication();

int main(int argc, char** argv)
{
	UME::Log::Init();
	UME_ENGINE_WARN("Inititalized Log");

	auto app = UME::CreateApplication();
	app->GameLoop();
	delete app;
}

#endif
