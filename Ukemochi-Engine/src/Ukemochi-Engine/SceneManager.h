/* Start Header ************************************************************************/
/*!
\file       SceneManager.h
\author     Tan Si Han, t.sihan, 2301264, t.sihan\@digipen.edu
\date       Nov 8, 2024
\brief      This file contains the declaration of the SceneManger system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include "PreCompile.h"
#include "ECS/ECS.h"
#include "Game/GSM.h"
#include "Factory/GameObject.h"
#include "InGameGUI/InGameGUI.h"

namespace Ukemochi
{
	class SceneManager
	{
	public:

		/*!***********************************************************************
		\brief
		 Making SceneManger class object as a Singleton object
		*************************************************************************/
		static SceneManager& GetInstance()
		{
			static std::unique_ptr<SceneManager> instance(new SceneManager());
			return *instance;
		}

		SceneManager();
		~SceneManager();

		void SceneMangerLoad();

		void SceneMangerInit();

		void SceneMangerUpdate();

		void SceneMangerRunSystems();

		void SceneMangerUpdateCamera(double deltaTime);

		void SceneManagerDraw();

		void SceneManagerFree();

		void SceneManagerUnload();

		bool& GetOnIMGUI();

		std::string& GetCurrScene();

		// Load save file to Scene
		void LoadSaveFile(const std::string& file_name);

		// Save the current scene to a file
		void SaveScene(const std::string& file_name);

		void CreateNewScene(const std::string& file_name);

		void SavePrefab(GameObject* prefabObj, const std::string& file_name);

		Vec2 GetPlayScreen();

		void SetPlayScreen(Vec2 playsize);

		void print_performance(std::chrono::duration<double> loop, std::chrono::duration<double> collision, std::chrono::duration<double> physics, std::chrono::duration<double> graphics);

		// --- Frame Performance Viewer
		// TODO: Quick Hack needs to be encapsulated
		static std::chrono::duration<double> loop_time;
		static std::chrono::duration<double> collision_time;
		static std::chrono::duration<double> physics_time;
		static std::chrono::duration<double> graphics_time;
		static std::chrono::duration<double> logic_time;

		std::chrono::steady_clock::time_point sys_start;
		std::chrono::steady_clock::time_point sys_end;
		std::chrono::steady_clock::time_point loop_start;
		std::chrono::steady_clock::time_point loop_end;
	private:
		std::string sceneName;
		Vec2 cameraSize;
	};
}