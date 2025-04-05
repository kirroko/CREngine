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
#include "PreCompile.h"
#include "SplashScreen.h"

#include "FrameController.h"
#include "Asset Manager/AssetManager.h"
#include "ECS/ECS.h"
#include "Job/JobSystem.h"

using namespace Ukemochi;

/*!***********************************************************************
\brief
 This function is responsible for loading resources and data of SplashScreen of the game.
*************************************************************************/
void SplashScreen::SplashScreenLoad()
{
	//ECS::GetInstance().GetSystem<AssetManager>()->addTexture("../Assets/Textures/Digipen_splashscreen.png");
	//ECS::GetInstance().GetSystem<AssetManager>()->addTexture("../Assets/Textures/Grp_Logo.png");
	//ECS::GetInstance().GetSystem<AssetManager>()->addTexture("../Assets/Textures/game_logo.png");

	//ECS::GetInstance().GetSystem<AssetManager>()->LoadAllTexture();

	splashTex = ECS::GetInstance().GetSystem<AssetManager>()->getTexture("../Assets/Textures/digipen_black.png");
	if (splashTex->ID <= 0)
		UME_ENGINE_ERROR("SplashTex missing!");
	logoTex = ECS::GetInstance().GetSystem<AssetManager>()->getTexture("../Assets/Textures/Grp_Logo.png");
	if (logoTex->ID <= 0)
		UME_ENGINE_ERROR("LogoTex missing!");
	titleTex = ECS::GetInstance().GetSystem<AssetManager>()->getTexture("../Assets/Textures/game_logo.png");
	if (titleTex->ID <= 0)
		UME_ENGINE_ERROR("TitleTex missing!");
}

/*!***********************************************************************
\brief
 This function is responsible for initializing the initial state of SplashScreen of the game.
*************************************************************************/
void SplashScreen::SplashScreenInit()
{
	float vertices[] = {
		// Positions         // Colors      // Texture Coords
		-0.5f, -0.5f, 0.0f,  1.0f,1.0f,1.0f,   0.0f, 0.0f,  // Bottom-left
		 0.5f, -0.5f, 0.0f,  1.0f,1.0f,1.0f,   1.0f, 0.0f,  // Bottom-right
		 0.5f,  0.5f, 0.0f,  1.0f,1.0f,1.0f,   1.0f, 1.0f,  // Top-right

		-0.5f, -0.5f, 0.0f,  1.0f,1.0f,1.0f,   0.0f, 0.0f,  // Bottom-left
		 0.5f,  0.5f, 0.0f,  1.0f,1.0f,1.0f,   1.0f, 1.0f,  // Top-right
		-0.5f,  0.5f, 0.0f,  1.0f,1.0f,1.0f,   0.0f, 1.0f   // Top-left
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // UV
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(8 * sizeof(float))); // Alpha
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
}

/*!***********************************************************************
\brief
 This function is responsible for updating the state of SplashScreen of the game.
*************************************************************************/
void SplashScreen::SplashScreenUpdate()
{
	auto checkInput = []() {
		return Input::IsKeyTriggered(UME_MOUSE_BUTTON_LEFT) || Input::IsKeyTriggered(UME_MOUSE_BUTTON_RIGHT) ||
			Input::IsKeyTriggered(UME_KEY_ENTER) || Input::IsKeyTriggered(UME_KEY_SPACE) ||
			Input::IsKeyTriggered(UME_KEY_ESCAPE);
		};

	auto updateAlpha = [&](float& alpha, float duration, bool& flag) {
		alpha = timer / duration;
		if (alpha >= 1.0f) {
			spd = -1.f;
		}
		if (checkInput()) {
			spd = -1.f;
			alpha = 0.f;
		}
		if (spd <= -0.9f && alpha <= 0.01f) {
			spd = 1.f;
			flag = true;
		}
		};

	timer += static_cast<float>(g_FrameRateController.GetDeltaTime()) * spd;

	if (!school) {
		updateAlpha(splash_alpha, splash_timer, school);
	}
	else if (!logo) {
		updateAlpha(logo_alpha, game_fade_timer, logo);
	}
	else {
		updateAlpha(title_alpha, game_fade_timer, animation);
	}
}

/*!***********************************************************************
\brief
 This function is responsible for drawing the graphics and rendering of SplashScreen of the game.
*************************************************************************/
void SplashScreen::SplashScreenFree()
{
}

/*!***********************************************************************
\brief
 This function is responsible for releasing resources of SplashScreen of the game.
*************************************************************************/
bool& SplashScreen::IsSplashScreenDone()
{
	return animation;
}

/*!***********************************************************************
\brief
 Get/Set when splashscreen done
*************************************************************************/
void SplashScreen::SplashScreenRender()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ECS::GetInstance().GetSystem<Renderer>()->splash_screen_shader_program->Activate();
	auto shader = ECS::GetInstance().GetSystem<Renderer>()->splash_screen_shader_program;
	// Get projection and view matrices from the camera
	glm::mat4 projection = ECS::GetInstance().GetSystem<Camera>()->getCameraProjectionMatrix();
	glm::mat4 view = ECS::GetInstance().GetSystem<Camera>()->getCameraViewMatrix();

	// Get screen dimensions
	float screenWidth = ECS::GetInstance().GetSystem<Camera>()->viewport_size.x;
	float screenHeight = ECS::GetInstance().GetSystem<Camera>()->viewport_size.y;


	int width = 1;
	int height = 1;
	if (!school)
	{
		width = splashTex->width;
		height = splashTex->height;
	}
	else if (!logo)
	{
		width = logoTex->width;
		height = logoTex->height;
	}
	else
	{
		width = titleTex->width;
		height = titleTex->height;
	}

	float posX = screenWidth * 0.5f;
	float posY = screenHeight * 0.5f;

	// translate scaling
	auto model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(posX, posY, 0.0f));
	model = glm::scale(model, glm::vec3(width, height, 1.0f));

	glm::mat4 mvp = projection * view * model;
	shader->setMat4("mvp", mvp);

	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	if (!school)
	{
		glBindTexture(GL_TEXTURE_2D, splashTex->ID);
		shader->setFloat("alpha", splash_alpha);
	}
	else if (!logo)
	{
		glBindTexture(GL_TEXTURE_2D, logoTex->ID);
		shader->setFloat("alpha", logo_alpha);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, titleTex->ID);
		shader->setFloat("alpha", title_alpha);
	}

	
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
}
