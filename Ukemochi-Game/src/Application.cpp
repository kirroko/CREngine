#include <Ukemochi-Engine.h>

class Sandbox : public Ukemochi::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{
	}
};

Ukemochi::Application* Ukemochi::CreateApplication()
{
	return new Sandbox();
}