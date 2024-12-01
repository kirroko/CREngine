/* Start Header ************************************************************************/
/*!
\file       Sandbox.cpp
\author     HURNG Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu
\date       Sept 25, 2024
\brief      This file defines the `Sandbox` class, a derived application
			from `Ukemochi::Application`, and provides the implementation
			for creating an instance of the application framework.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

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

/*!***********************************************************************
\brief
 Creates an instance of the `Sandbox` application. This function serves as
 the entry point for the `Ukemochi::Application` framework to initialize
 the user-defined application.
\return
 A pointer to a new instance of the `Sandbox` class.
*************************************************************************/
Ukemochi::Application* Ukemochi::CreateApplication()
{
	return new Sandbox();
}