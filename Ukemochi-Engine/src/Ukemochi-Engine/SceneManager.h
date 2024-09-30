#pragma once
#include "PreCompile.h"
#include "ECS/ECS.h"
#include "Game/GSM.h"
#include "SceneList.h"

namespace Ukemochi
{
	class Scene;

	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		// Load the scene from a file or data source
		void LoadScene();

		// Load save file to Scene
		void LoadSaveFile(const std::string& sceneFile);

		// Initialize the scene entities and systems
		void InitScene();

		// Update the scene and its entities
		void Update(float deltaTime);

		// Clear current scene (entities and resources)
		void ClearScene();

		// Save the current scene to a file
		void SaveScene(int saveFile);

		// Optional: Transition to a different scene
		void TransitionToScene();

	private:
		int currentScene;
	};

	class Scene
	{
	public:
		Scene(SceneLists name);
		~Scene();

		void AddEntity(EntityID entity);
		void RemoveEntity(EntityID entity);
		void ClearEntities();

		// Load resources (textures, meshes, etc.)
		void LoadResources();
		// Unload resources when the scene is no longer needed
		void UnloadResources();

		int GetSceneName();

	private:
		SceneLists sceneName;
		// You can add resource managers for textures, sounds, etc.
	};
}