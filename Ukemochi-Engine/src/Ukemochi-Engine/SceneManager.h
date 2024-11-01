#pragma once
#include "PreCompile.h"
#include "ECS/ECS.h"
#include "Game/GSM.h"

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

		void LoadAndInitScene();

		// Load the scene from a file or data source
		void LoadScene();

		// Load save file to Scene
		void LoadSaveFile(const std::string& sceneFile);

		// Initialize the scene entities and systems
		void InitScene();

		// Update the scene and its entities
		void Update(double deltaTime);

		void UpdateScenes();

		// Clear current scene (entities and resources)
		void ClearScene();

		// Save the current scene to a file
		void SaveScene();

		// Optional: Transition to a different scene
		void TransitionToScene();

	private:
	};
}