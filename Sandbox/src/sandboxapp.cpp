#include <CR.h>

class Sandbox : public CR::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};

CR::Application* CR::CreateApplication()
{
	return new Sandbox();
}