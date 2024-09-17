#pragma once

#ifdef UME_PLATFORM_WINDOWS

extern UME::Application* UME::CreateApplication();

int main(int argc, char** argv)
{
	UME::Log::Init();
	UME_ENGINE_WARN("Inititalized Log");
	int a = 5;
	UME_INFO("Yes! Var = {0}", a);

	auto app = UME::CreateApplication();
	app->GameLoop();
	delete app;
}

#endif
