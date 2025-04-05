/* Start Header ************************************************************************/
/*!
\file       SplashScreen.cpp
\author		Wong Jun Yu, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date       April 5, 2025
\brief      This file contains definition functions for SplashScreen Scene

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#pragma once
#include "glad/glad.h"
#include "Graphics/Texture.h"

namespace Ukemochi
{
	class SplashScreen
	{
		std::shared_ptr<Texture> splashTex;
		std::shared_ptr<Texture> logoTex;
		std::shared_ptr<Texture> titleTex;

		GLuint VAO{}, VBO{};

		float timer = 0.f;
		const float splash_timer = 2.0f;
		const float game_fade_timer = 1.f;
		float splash_alpha = 1.f;
		float title_alpha = 0.f;
		float logo_alpha = 0.f;
		float spd = 1.f;

		bool school = false;
		bool logo = false;
		bool animation = false;
	public:
		/*!***********************************************************************
		\brief
		 Load the resources and data of SplashScreen of the game.
		*************************************************************************/
		void SplashScreenLoad();
		/*!***********************************************************************
		\brief
		 Initialize the initial state of SplashScreen of the game.
		*************************************************************************/
		void SplashScreenInit();
		/*!***********************************************************************
		\brief
		 Update the state of SplashScreen of the game.
		*************************************************************************/
		void SplashScreenUpdate();
		/*!***********************************************************************
		\brief
		 This function is responsible for drawing the graphics and rendering of SplashScreen of the game.
		*************************************************************************/
		void SplashScreenFree();
		/*!***********************************************************************
		\brief
		 This function is responsible for releasing resources of SplashScreen of the game.
		*************************************************************************/
		bool& IsSplashScreenDone();
		/*!***********************************************************************
		\brief
		 Get/Set when splashscreen done
		*************************************************************************/
		void SplashScreenRender();
	};
}
