#include "PreCompile.h"
#include "SceneManager.h"
#include "Audio/Audio.h"
#include "Physics/Physics.h"
#include "Collision/Collision.h"
#include "Graphics/Renderer.h"

namespace Ukemochi
{
	SceneManager::SceneManager()
	{
		Audio audio;
		// Set up ECS
		ECS::GetInstance().Init();

		// Register your components
		ECS::GetInstance().RegisterComponent<Transform>();
		ECS::GetInstance().RegisterComponent<Rigidbody2D>();
		ECS::GetInstance().RegisterComponent<BoxCollider2D>();
		ECS::GetInstance().RegisterComponent<CircleCollider2D>();
		ECS::GetInstance().RegisterComponent<SpriteRender>();

		// Register your systems
		ECS::GetInstance().RegisterSystem<Physics>();
		ECS::GetInstance().RegisterSystem<Collision>();
		ECS::GetInstance().RegisterSystem<Renderer>();

		// Set a signature to your system
		// Each system will have a signature to determine which entities it will process

		// For physics system
		SignatureID sig;
		sig.set(ECS::GetInstance().GetComponentType<Transform>());
		sig.set(ECS::GetInstance().GetComponentType<Rigidbody2D>());
		ECS::GetInstance().SetSystemSignature<Physics>(sig);

		// For renderer system
		sig.reset();
		sig.set(ECS::GetInstance().GetComponentType<Transform>());
		sig.set(ECS::GetInstance().GetComponentType<SpriteRender>());
		ECS::GetInstance().SetSystemSignature<Renderer>(sig);

		// For collision system
		sig.reset();
		sig.set(ECS::GetInstance().GetComponentType<Transform>());
		sig.set(ECS::GetInstance().GetComponentType<BoxCollider2D>());
		ECS::GetInstance().SetSystemSignature<Collision>(sig);
	
		//init GSM
		GSM_Initialize(GS_LEVEL1);
		currentScene = GS_LEVEL1;
		//OnGameStateChanged(SL_LEVEL1);
	}

	SceneManager::~SceneManager()
	{
		ClearScene();
	}

	void SceneManager::LoadScene()
	{
		// Clear any existing scene
		ClearScene();

		//update GSM
		GSM_Update();

		// Load the new scene from a file
		currentScene = gsm_current;


		// Load resources associated with the scene
		gsm_fpLoad();

		//if save file exist
		//look for save file
		//if exist
		
		//LoadSaveFile(sceneFile);

		// Initialize the scene (add entities, etc.)
		//InitScene();
	}

	void SceneManager::LoadSaveFile(const std::string& sceneFile)
	{
		// Load scene from file
		std::ifstream file(sceneFile);
		if (!file.is_open())
		{
			// Handle error, maybe fall back to an empty scene
			std::cerr << "Failed to open scene file: " << sceneFile << std::endl;
			return;
		}

		std::string line;
		while (std::getline(file, line))
		{
			std::istringstream iss(line);
			std::string type;
			iss >> type;

			if (type == "Entity")
			{
				EntityID entity = ECS::GetInstance().CreateEntity();

				// Read entity's components
				std::string componentType;
				while (iss >> componentType)
				{
					//if (componentType == "Position")
					//{
					//	float x, y;
					//	iss >> x >> y;
					//	ECS::GetInstance().addComponent<PositionComponent>(entity, PositionComponent(x, y));
					//}
					//else if (componentType == "Health")
					//{
					//	int health;
					//	iss >> health;
					//	ECS::GetInstance().addComponent<HealthComponent>(entity, HealthComponent(health));
					//}
					// Add more component types here
				}

				// Add the entity to the scene
				//currentScene->AddEntity(entity);
			}
		}

		file.close();
	}

	void SceneManager::InitScene()
	{
		if (currentScene != gsm_current)
		{
			// No current scene to initialize
			return;
		}

		//init ECS here e.g. player

	}

	void SceneManager::Update(float deltaTime)
	{
		if (currentScene!=gsm_current || gsm_current == GS_PAUSED)
		{
			// No scene to update OR Do not update if paused
			return;
		}

		//current state != restart
		if (gsm_current != GS_STATES::GS_RESTART)
		{
			//load scene
			
			GSM_Update();
			gsm_fpLoad();
		}
		else
		{
			gsm_next = gsm_current = gsm_previous;
		}

		//init scene
		gsm_fpInitialize();

		while (gsm_current == gsm_next)
		{
			//Update and Draw current scene
			gsm_fpUpdate();
			//update ECS
			gsm_fpDraw();
		}
		//Free scene
		gsm_fpFree();

		//If game not restart unload Scene
		if (gsm_next != GS_STATES::GS_RESTART)
		{
			gsm_fpUnload();
		}

		gsm_previous = gsm_current = gsm_next;
		//OnGameStateChanged(gsm_next);
	}

	void SceneManager::ClearScene()
	{
		if (currentScene == gsm_current)
		{
			gsm_fpFree();
			// Unload the resources associated with the current scene
			//currentScene->UnloadResources();

			// Clear all entities in the scene
			//currentScene->ClearEntities();

			// Clear the ECS entities and components
			

			// Reset the scene
			//currentScene.reset();
		}
	}

	void SceneManager::SaveScene(int saveFile)
	{
		if (currentScene != gsm_current)
		{
			// No scene to save
			return;
		}

		// Example: Saving scene logic to file (pseudo-code)
		// This can be extended with proper serialization logic for saving entity states, components, etc.
		//std::ofstream file(saveFile);
		//if (file.is_open())
		{
			//file << "Scene Name: " << currentScene->GetName() << std::endl;
			// Save each entity and its components
			// Add your specific logic for saving entity data
		//	file.close();
		}
	}

	void SceneManager::TransitionToScene()
	{
		// Clear the current scene and load the new one
		LoadScene();
	}


//#Scene

	Scene::Scene(SceneLists curr)
	{
		sceneName = curr;
	}

	Scene::~Scene()
	{
		ClearEntities();
	}

	void Scene::AddEntity(EntityID entity)
	{
		// Add the entity to the ECS (this is optional since ECS might already manage the entities globally)
		// However, keeping a local list of entity IDs within the scene can be helpful for cleanup or scene-specific actions
	}

	void Scene::RemoveEntity(EntityID entity)
	{
		// Remove the entity from the ECS
	}

	void Scene::ClearEntities()
	{
		// Assuming the ECS can clear scene-specific entities or we keep track of scene entities,
		// you would clear them here. Since ECS is global, you can just call ECS::Get().clear() if needed.

	}

	void Scene::LoadResources()
	{
		// Example resource loading logic
		// Load textures, meshes, sounds, etc.
		// Use a resource manager or asset loader
		// Pseudo-code:
		// ResourceManager::Get().loadTexture("player.png");
		// ResourceManager::Get().loadMesh("level1.obj");

	}

	void Scene::UnloadResources()
	{
		// Example resource unloading logic
		// Pseudo-code:
		// ResourceManager::Get().unloadTexture("player.png");
		// ResourceManager::Get().unloadMesh("level1.obj");

	}

	int Scene::GetSceneName()
	{
		return sceneName;
	}

}