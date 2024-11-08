/* Start Header
*****************************************************************/
/*!
\file       SceneManager.h
\author     Tan Si Han, t.sihan, 2301264
\par        email: t.sihan\@digipen.edu
\date       Nov 8, 2024
\brief      This file contains the declaration of the SceneManger system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

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

		//void LoadAndInitScene();

		//// Load the scene from a file or data source
		//void LoadScene();

		// Load save file to Scene
		void LoadSaveFile(const std::string& file_name);

		//// Initialize the scene entities and systems
		//void InitScene();

		//// Update the scene and its entities
		//void Update(double deltaTime);

		//void UpdateScenes(double deltaTime);

		//// Clear current scene (entities and resources)
		//void ClearScene();

		// Save the current scene to a file
		void SaveScene(const std::string& file_name);

		void CreateNewScene(const std::string& file_name);

		void SavePrefab(GameObject* prefabObj, const std::string& file_name);

		Vec2 GetPlayScreen();

		void SetPlayScreen(Vec2 playsize);

		void print_performance(std::chrono::duration<double> loop, std::chrono::duration<double> collision, std::chrono::duration<double> physics, std::chrono::duration<double> graphics);

	private:
		std::string sceneName;
		Vec2 cameraSize;
	};
}