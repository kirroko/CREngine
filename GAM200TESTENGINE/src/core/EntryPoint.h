#pragma once

#ifdef CR_PLATFORM_WINDOWS

extern CR::Application* CR::CreateApplication();

int main(int argc, char** argv)
{
	CR::Log::Init();
	CR_ENGINE_WARN("Inititalized Log");
	int a = 5;
	CR_INFO("Yes! Var = {0}",a);

	auto app = CR::CreateApplication();
	app->run();
	delete app;
}

#endif