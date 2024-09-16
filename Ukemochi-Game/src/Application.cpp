#include <Ukemochi-Engine.h>

class Sandbox : public UME::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{
	}
};

UME::Application* UME::CreateApplication()
{
	return new Sandbox();
}