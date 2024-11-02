#include "PreCompile.h"
#include "SceneManager.h"
#include "Audio/Audio.h"
#include "Physics/Physics.h"
#include "Collision/Collision.h"
#include "Graphics/Renderer.h"
#include "Logic/Logic.h"

namespace Ukemochi
{
	SceneManager::SceneManager()
	{
		//Audio audio;
		// Set up ECS
		ECS::GetInstance().Init();

		// TODO: Register your components
		ECS::GetInstance().RegisterComponent<Transform>();
		ECS::GetInstance().RegisterComponent<Rigidbody2D>();
		ECS::GetInstance().RegisterComponent<BoxCollider2D>();
		ECS::GetInstance().RegisterComponent<CircleCollider2D>();
		ECS::GetInstance().RegisterComponent<SpriteRender>();
		ECS::GetInstance().RegisterComponent<Script>();


		// TODO: Register your systems
		ECS::GetInstance().RegisterSystem<Physics>();
		ECS::GetInstance().RegisterSystem<Collision>();
		ECS::GetInstance().RegisterSystem<Renderer>();
		ECS::GetInstance().RegisterSystem<LogicSystem>();
		//ECS::GetInstance().RegisterSystem<Audio>();

		// ScriptingEngine::GetInstance().CompileScriptAssembly();

		// TODO: Set a signature to your system
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

		// For Logic System
		sig.reset();
		sig.set(ECS::GetInstance().GetComponentType<Script>());
		ECS::GetInstance().SetSystemSignature<LogicSystem>(sig);
	
		//init GSM
		GSM_Initialize(GS_ENGINE);
	}

	SceneManager::~SceneManager()
	{
	
	}

	void SceneManager::LoadScene()
	{
		//update GSM
		GSM_Update();

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
				//EntityID entity = ECS::GetInstance().CreateEntity();

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
		gsm_fpInitialize();
		//init ECS here e.g. player

	}

	void SceneManager::Update(double deltaTime)
	{
		(void)deltaTime;
		//Update
		gsm_fpUpdate();
		//update ECS

		//Draw
		gsm_fpDraw();
	}

	void SceneManager::ClearScene()
	{
		gsm_fpUnload();
		// Clear the ECS entities and components
	}

	void SceneManager::SaveScene()
	{	
		const std::string saveFile = "save.txt";
		// Open file for writing
		std::ofstream file(saveFile);
		if (file.is_open())
		{
			// Save the name of the current scene
			file << "Scene Name: " << static_cast<GS_STATES>(gsm_current) << std::endl;

			// Iterate through each entity in the scene
			//for (auto& entity : //ECS)
			//{
			//	file << "Entity ID: " << entity.GetID() << std::endl;

			//	// Save each component of the entity
			//	for (auto& component : entity.GetComponents())
			//	{
			//		file << "Component: " << component.GetType() << std::endl;
			//		// Serialize component-specific data
			//		component.Serialize(file);
			//	}
			//}

			// Close the file
			file.close();
		}
		else
		{
			std::cerr << "Failed to open file: " << saveFile << std::endl;
		}
	}

	void SceneManager::TransitionToScene()
	{
		// Clear the current scene and load the new one
		LoadScene();
	}
}