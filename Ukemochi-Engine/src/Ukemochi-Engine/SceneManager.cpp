#include "PreCompile.h"
#include "SceneManager.h"
#include "Audio/Audio.h"
#include "Physics/Physics.h"
#include "Collision/Collision.h"
#include "Math/Transformation.h"
#include "Graphics/Renderer.h"
#include "Serialization/Serialization.h"
#include "Logic/DataSyncSystem.h"
#include "Logic/Logic.h"
#include "Graphics/Camera2D.h"
#include "Factory/Factory.h"
#include "Factory/GameObjectManager.h"
#include "ImGui/ImGuiCore.h"
#include "InGameGUI/InGameGUI.h"

namespace Ukemochi
{
	using namespace rapidjson;

	const float SPRITE_SCALE = 100.f;
	const float ENTITY_ACCEL = 750.f;
	const float PLAYER_FORCE = 750.f;

	SceneManager::SceneManager()
		:sceneName("NewScene")
	{
		es_current = ES_ENGINE;
		//Audio audio;
		Audio::GetInstance();
		// Set up ECS
		ECS::GetInstance().Init();

		// TODO: Register your components
		ECS::GetInstance().RegisterComponent<Transform>();
		ECS::GetInstance().RegisterComponent<Rigidbody2D>();
		ECS::GetInstance().RegisterComponent<BoxCollider2D>();
		ECS::GetInstance().RegisterComponent<CircleCollider2D>();
		ECS::GetInstance().RegisterComponent<SpriteRender>();
		ECS::GetInstance().RegisterComponent<Script>();
		ECS::GetInstance().RegisterComponent<Button>();

		// TODO: Register your systems
		ECS::GetInstance().RegisterSystem<Physics>();
		ECS::GetInstance().RegisterSystem<Collision>();
		ECS::GetInstance().RegisterSystem<Transformation>();
		ECS::GetInstance().RegisterSystem<Renderer>();
		ECS::GetInstance().RegisterSystem<LogicSystem>();
		ECS::GetInstance().RegisterSystem<DataSyncSystem>();
		ECS::GetInstance().RegisterSystem<Camera>();
		ECS::GetInstance().RegisterSystem<InGameGUI>();
		//ECS::GetInstance().RegisterSystem<Audio>();

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
	
		////init GSM
		//GSM_Initialize(GS_LEVEL1);
		std::cout << "System Up and Running" << std::endl;

		// For Logic System
		sig.reset();
		sig.set(ECS::GetInstance().GetComponentType<Script>());
		ECS::GetInstance().SetSystemSignature<LogicSystem>(sig);

		sig.reset();
		sig.set(ECS::GetInstance().GetComponentType<Transform>());
		sig.set(ECS::GetInstance().GetComponentType<Rigidbody2D>());
		ECS::GetInstance().SetSystemSignature<DataSyncSystem>(sig);
	

		// For in game GUI system
		sig.reset();
		sig.set(ECS::GetInstance().GetComponentType<Transform>());
		sig.set(ECS::GetInstance().GetComponentType<Button>());
		ECS::GetInstance().SetSystemSignature<InGameGUI>(sig);

		//init GSM
		//GSM_Initialize(GS_ENGINE);
	}

	SceneManager::~SceneManager()
	{

	}

	void SceneManager::SceneMangerLoad()
	{
		//load all assest
		// load textures
		Audio::GetInstance().LoadSound(R"(../Assets/Audio/BGM_game.mp3)");
		Audio::GetInstance().LoadSound(R"(../Assets/Audio/SFX_jump.wav)");

		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/Moon Floor.png"); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/Worm.png"); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/Bunny_Right_Sprite.png"); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/terrain.png"); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/UI/pause.png"); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/UI/base.png"); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/UI/game_logo.png"); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/idle_player_sprite_sheet.png"); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/running_player_sprite_sheet.png"); // load texture

		UseImGui::LoadScene();

		if (UseImGui::GetSceneSize() == 0)
		{
			SaveScene("NewScene");
		}
		else
		{
			//LoadSaveFile(GetCurrScene());
		}
	}

	void SceneManager::SceneMangerInit()
	{
		// Initialize the graphics and collision system
		ECS::GetInstance().GetSystem<Renderer>()->init();
		ECS::GetInstance().GetSystem<Collision>()->Init();

		// Set the player object in the Renderer
		//ECS::GetInstance().GetSystem<Renderer>()->SetPlayerObject(player_obj);

		//// Set the player object in the Transformation
		//ECS::GetInstance().GetSystem<Transformation>()->playerObject = &player_obj;

		// Initialize the in game GUI system
		//ECS::GetInstance().GetSystem<InGameGUI>()->Init();
	}

	void SceneManager::SceneMangerUpdate()
	{
		ECS::GetInstance().GetSystem<InGameGUI>()->Update();

		ECS::GetInstance().GetSystem<Transformation>()->ComputeTransformations();

		ECS::GetInstance().GetSystem<Renderer>()->animationKeyInput();

		SceneManagerDraw();
	}

	void SceneManager::SceneMangerRunSystems()
	{
		if (ECS::GetInstance().GetSystem<Transformation>()->player != -1)
		{
			GameObject* playerObj = &GameObjectManager::GetInstance().GetGO(ECS::GetInstance().GetSystem<Transformation>()->player);
			// --- HANDLE USER INPUTS ---

		// Player Inputs for movement
			auto& player_rb = playerObj->GetComponent<Rigidbody2D>();
			// Press 'W' or up key to move the player up
			if (Input::IsKeyPressed(UME_KEY_W))
				ECS::GetInstance().GetSystem<Physics>()->AddForceY(player_rb, PLAYER_FORCE);
			// Press 'S' or down key to move the player down
			else if (Input::IsKeyPressed(UME_KEY_S))
				ECS::GetInstance().GetSystem<Physics>()->AddForceY(player_rb, -PLAYER_FORCE);
			else
				ECS::GetInstance().GetSystem<Physics>()->RemoveForceY(player_rb); // Stop moving the player in the y axis

			// Press 'A' or left key to move the player left
			if (Input::IsKeyPressed(UME_KEY_A))
			{
				ECS::GetInstance().GetSystem<Physics>()->AddForceX(player_rb, -PLAYER_FORCE);
				ECS::GetInstance().GetSystem<Transformation>()->isFacingRight = false;
			}
			// Press 'D' or right key to move the player to the right
			else if (Input::IsKeyPressed(UME_KEY_D))
			{
				ECS::GetInstance().GetSystem<Physics>()->AddForceX(player_rb, PLAYER_FORCE);
				ECS::GetInstance().GetSystem<Transformation>()->isFacingRight = true;
			}
			else
				ECS::GetInstance().GetSystem<Physics>()->RemoveForceX(player_rb); // Stop moving the player in the x axis

			// Input for rotation, to test rotate physics
			if (Input::IsKeyPressed(UME_KEY_R))
				ECS::GetInstance().GetSystem<Physics>()->AddTorque(player_rb, -PLAYER_FORCE);
			else if (Input::IsKeyPressed(UME_KEY_T))
				ECS::GetInstance().GetSystem<Physics>()->AddTorque(player_rb, PLAYER_FORCE);
			else
				ECS::GetInstance().GetSystem<Physics>()->RemoveTorque(player_rb);

			// Input for scaling, to test scaling of the player
			auto& player_trans = playerObj->GetComponent<Transform>();
			if (Input::IsKeyPressed(UME_KEY_F))
				ECS::GetInstance().GetSystem<Transformation>()->IncreaseScale(player_trans);
			else if (Input::IsKeyPressed(UME_KEY_G))
				ECS::GetInstance().GetSystem<Transformation>()->DecreaseScale(player_trans);
		}
		

		// Renderer Inputs
		/*if (UME::Input::IsKeyTriggered(GLFW_KEY_T))
		if (Ukemochi::Input::IsKeyTriggered(GLFW_KEY_T))
			ECS::GetInstance().GetSystem<Renderer>()->ToggleInputsForScale();
		else if (Ukemochi::Input::IsKeyTriggered(GLFW_KEY_Y))
			ECS::GetInstance().GetSystem<Renderer>()->ToggleInputsForRotation();
		else*/
		if (Input::IsKeyTriggered(UME_KEY_U))
			ECS::GetInstance().GetSystem<Renderer>()->debug_mode_enabled = static_cast<GLboolean>(!ECS::GetInstance().GetSystem<Renderer>()->debug_mode_enabled);

		// Audio Inputs
		//if (Ukemochi::Input::IsKeyTriggered(GLFW_KEY_P))
		//{
		//	audioVolume -= 0.02f;
		//	audioVolume = audioVolume < 0.f ? 0.f : audioVolume;
		//	Audio::GetInstance().SetAudioVolume(BGM, audioVolume);
		//}
		//if (Ukemochi::Input::IsKeyTriggered(GLFW_KEY_O))
		//{
		//	audioVolume += 0.02f;
		//	audioVolume = audioVolume > 1.f ? 1.f : audioVolume;
		//	Audio::GetInstance().SetAudioVolume(BGM, audioVolume);
		//}
		if (Ukemochi::Input::IsKeyPressed(GLFW_KEY_M))
		{
			Audio::GetInstance().StopAllSoundsInGroup(LEVEL1);
		}
		if (Ukemochi::Input::IsKeyPressed(GLFW_KEY_N))
		{
			Audio::GetInstance().PlayAllSoundsInGroup(LEVEL1);
		}

		ECS::GetInstance().GetSystem<InGameGUI>()->Update();
		ECS::GetInstance().GetSystem<Renderer>()->animationKeyInput();

		// --- PHYSICS UPDATE ---
		// Update the entities physics
		ECS::GetInstance().GetSystem<Physics>()->UpdatePhysics();

		// --- COLLISION UPDATE ---
		// Check the collisions between the entities
		ECS::GetInstance().GetSystem<Collision>()->CheckCollisions();

		// --- TRANSFORMATION UPDATE ---
		// Compute the entities transformations
		ECS::GetInstance().GetSystem<Transformation>()->ComputeTransformations();
		
		SceneManagerDraw();
	}

	void SceneManager::SceneMangerUpdateCamera(double deltaTime)
	{
		// Camera
		ECS::GetInstance().GetSystem<Camera>()->processCameraInput(deltaTime);
	}

	void SceneManager::SceneManagerDraw()
	{
		//Draw
		ECS::GetInstance().GetSystem<Renderer>()->renderToFramebuffer();
	}

	void SceneManager::SceneManagerFree()
	{
		std::vector<GameObject*> list = GameObjectManager::GetInstance().GetAllGOs();
		for (auto& gameobject : list)
		{
			GameObjectManager::GetInstance().DestroyObject(gameobject->GetInstanceID());
		}
		ECS::GetInstance().ReloadEntityManager();
		Audio::GetInstance().StopAudioGroup(ChannelGroups::LEVEL1);
	}

	void SceneManager::SceneManagerUnload()
	{
		ECS::GetInstance().GetSystem<Renderer>()->cleanUp();
	}

	bool& SceneManager::GetOnIMGUI()
	{
		static bool isOnIMGUI = false;
		return isOnIMGUI;
	}

	std::string& SceneManager::GetCurrScene()
	{
		return GetInstance().sceneName;
	}

	//void SceneManager::LoadAndInitScene()
	//{
	//	//current state != restart
	//	if (gsm_current != GS_STATES::GS_RESTART)
	//	{
	//		//LoadScene();
	//	}
	//	else
	//	{
	//		gsm_next = gsm_current = gsm_previous;
	//	}

	//	//Init Scene
	//	InitScene();
	//}

	//void SceneManager::LoadScene()
	//{
	//	//update GSM
	//	GSM_Update();

	//	// Load resources associated with the scene
	//	gsm_fpLoad();

	//	//if save file exist
	//	//look for save file
	//	//if exist
	//	
	//	//LoadSaveFile(sceneFile);
	//}

	void SceneManager::LoadSaveFile(const std::string& file_name)
	{
		std::string sceneCpy = file_name;
		GetInstance().sceneName = file_name;
		size_t extensionPos = sceneCpy.find(".json");
		if (extensionPos != std::string::npos) {
			GetInstance().sceneName = sceneCpy.substr(0, extensionPos);
		}
		//unload curr scene
		SceneManagerFree();

		std::string file_path = "../Assets/Scenes/" + file_name;
		Document document;

		if (!Serialization::LoadJSON(file_path, document))
		{
			std::cerr << "Failed to load scene from file: " << file_path << std::endl;
			return;
		}

		if (!document.HasMember("GameObjects") || !document["GameObjects"].IsArray())
		{
			std::cerr << "Invalid scene file format: " << file_path << std::endl;
			return;
		}

		const auto& gameObjectsArray = document["GameObjects"].GetArray();

		// Iterate through each GameObject in the JSON array
		for (const auto& gameObjectData : gameObjectsArray)
		{
			std::string name = gameObjectData["Name"].GetString();
			std::string tag = gameObjectData["Tag"].GetString();

			// Create a new GameObject and add it to the scene
			
			GameObject& newObject = GameObjectManager::GetInstance().CreateObject(name,tag);
			//GameObject newObject = GameObject(entity,name, tag);

			// Iterate through components and add them based on their type
			for (const auto& componentData : gameObjectData["Components"].GetArray())
			{
				std::string componentName = componentData["Name"].GetString();

				if (componentName == "Transform")
				{
					Vec2 position(
						componentData["Position"][0].GetFloat(),
						componentData["Position"][1].GetFloat()
					);
					float rotation = componentData["rotation"].GetFloat();
					Vec2 scale(
						componentData["Scale"][0].GetFloat(),
						componentData["Scale"][1].GetFloat()
					);
					
					if (!newObject.HasComponent<Transform>())
					{
						newObject.AddComponent<Transform>({ Mtx44(), position, rotation, scale });
					}

				}
				else if (componentName == "Rigidbody2D")
				{
					Vec2 position(
						componentData["Position"][0].GetFloat(),
						componentData["Position"][1].GetFloat()
					);
					Vec2 velocity(
						componentData["Velocity"][0].GetFloat(),
						componentData["Velocity"][1].GetFloat()
					);
					Vec2 acceleration(
						componentData["Acceleration"][0].GetFloat(),
						componentData["Acceleration"][1].GetFloat()
					);
					Vec2 force(
						componentData["Force"][0].GetFloat(),
						componentData["Force"][1].GetFloat()
					);
					float mass = componentData["Mass"].GetFloat();
					float inverseMass = componentData["Inverse Mass"].GetFloat();
					float linearDrag = componentData["Linear Drag"].GetFloat();
					float angle = componentData["Angle"].GetFloat();
					float angularVelocity = componentData["Angular Velocity"].GetFloat();
					float angularAcceleration = componentData["Angular Acceleration"].GetFloat();
					float torque = componentData["Torque"].GetFloat();
					float inertiaMass = componentData["Inertia Mass"].GetFloat();
					float inverseInertiaMass = componentData["Inverse Inertia Mass"].GetFloat();
					float angularDrag = componentData["Angular Drag"].GetFloat();
					bool useGravity = componentData["use_gravity"].GetBool();
					bool isKinematic = componentData["is_kinematic"].GetBool();
					if (!newObject.HasComponent<Rigidbody2D>())
					{
						newObject.AddComponent<Rigidbody2D>({ position, velocity, acceleration, force, mass, inverseMass, linearDrag,
															 angle, angularVelocity, angularAcceleration, torque, inertiaMass,
															 inverseInertiaMass, angularDrag, useGravity, isKinematic });
					}
				}
				else if (componentName == "BoxCollider2D")
				{
					Vec2 min(
						componentData["Min"][0].GetFloat(),
						componentData["Min"][1].GetFloat()
					);
					Vec2 max(
						componentData["Max"][0].GetFloat(),
						componentData["Max"][1].GetFloat()
					);
					int collisionFlag = componentData["Collision Flag"].GetInt();
					bool isTrigger = componentData["is_trigger"].GetBool();
					std::string tag = componentData["Tag"].GetString();
					if (!newObject.HasComponent<BoxCollider2D>())
					{
						newObject.AddComponent<BoxCollider2D>({ min, max, collisionFlag, isTrigger, tag });
					}
				}
				else if (componentName == "CircleCollider2D")
				{
					Vec2 center(
						componentData["Center"][0].GetFloat(),
						componentData["Center"][1].GetFloat()
					);
					float radius = componentData["Radius"].GetFloat();
					if (!newObject.HasComponent<CircleCollider2D>())
					{
						newObject.AddComponent<CircleCollider2D>({ center, radius });
					}
				}
				else if (componentName == "SpriteRender")
				{
					std::string texturePath = componentData["Sprite"].GetString();
					SPRITE_SHAPE shape = componentData["Shape"].GetInt() == 0 ? SPRITE_SHAPE::BOX : SPRITE_SHAPE::CIRCLE;
					if (!newObject.HasComponent<SpriteRender>())
					{
						newObject.AddComponent<SpriteRender>({ texturePath, shape });
					}
					//ECS::GetInstance().GetSystem<Renderer>()->setUpTextures(newObject.GetComponent<SpriteRender>().texturePath);
					if (tag == "Player")
					{
						newObject.GetComponent<SpriteRender>().animated = true;
					}
				}
				else
				{
					std::cerr << "Unknown component type: " << componentName << std::endl;
				}

				if (tag == "Player")
				{
					ECS::GetInstance().GetSystem<Transformation>()->player = newObject.GetInstanceID();

					ECS::GetInstance().GetSystem<Renderer>()->SetPlayer(newObject.GetInstanceID());
					ECS::GetInstance().GetSystem<Renderer>()->SetPlayerObject(newObject);
					ECS::GetInstance().GetSystem<Renderer>()->initAnimationEntities();
				}
			}
		}

		std::cout << "Scene loaded successfully from file: " << file_path << std::endl;
	
	}

	//void SceneManager::InitScene()
	//{
	//	//gsm_fpInitialize();
	//	//init ECS here e.g. player
	//}

	//void SceneManager::Update(double deltaTime)
	//{
	//	(void)deltaTime;
	//	//gsm_fpDraw();
	//}

	//void SceneManager::UpdateScenes(double deltaTime)
	//{
	//	(void)deltaTime;
	//	//Update
	//	//gsm_fpUpdate();
	//	//update ECS

	//	//Draw
	//	//gsm_fpDraw();
	//}

	//void SceneManager::ClearScene()
	//{
	//	gsm_fpUnload();
	//	// Clear the ECS entities and components
	//}

	void SceneManager::SaveScene(const std::string& file_name)
	{	
		//get file name to save
		Document document;
		document.SetObject();
		Document::AllocatorType& allocator = document.GetAllocator();

		// Create a JSON array to hold game object data
		Value gameObjectsArray(rapidjson::kArrayType);

		std::vector<GameObject*> list = GameObjectManager::GetInstance().GetAllGOs();
		for (auto& gameobject : list)
		{
			//set the type
			Value gameObjectData(rapidjson::kObjectType);
			gameObjectData.AddMember("Name", Value(gameobject->GetName().c_str(), allocator), allocator);
			gameObjectData.AddMember("Tag", Value(gameobject->GetTag().c_str(),allocator), allocator);

			//set the type
			Value componentsArray(rapidjson::kArrayType);

			if (gameobject->HasComponent<Transform>())
			{
				Value transformComponent(rapidjson::kObjectType);
				
				transformComponent.AddMember("Name", Value("Transform", allocator), allocator);
				
				const auto& transform = gameobject->GetComponent<Transform>();
				Value position(rapidjson::kArrayType);
				position.PushBack(transform.position.x, allocator);
				position.PushBack(transform.position.y, allocator);
				transformComponent.AddMember("Position", position, allocator);

				transformComponent.AddMember("rotation", transform.rotation, allocator);
				
				Value scale(rapidjson::kArrayType);
				scale.PushBack(transform.scale.x, allocator);
				scale.PushBack(transform.scale.y, allocator);
				transformComponent.AddMember("Scale", scale, allocator);
				
				componentsArray.PushBack(transformComponent, allocator);
			}

			if (gameobject->HasComponent<Rigidbody2D>())
			{
				Value rigidbodyComponent(rapidjson::kObjectType);
				rigidbodyComponent.AddMember("Name", "Rigidbody2D", allocator);

				const auto& rigidbody = gameobject->GetComponent<Rigidbody2D>();
				Value position(rapidjson::kArrayType);
				position.PushBack(rigidbody.position.x, allocator).PushBack(rigidbody.position.y, allocator);
				rigidbodyComponent.AddMember("Position", position, allocator);

				Value velocity(rapidjson::kArrayType);
				velocity.PushBack(rigidbody.velocity.x, allocator).PushBack(rigidbody.velocity.y, allocator);
				rigidbodyComponent.AddMember("Velocity", velocity, allocator);

				Value acceleration(rapidjson::kArrayType);
				acceleration.PushBack(rigidbody.acceleration.x, allocator).PushBack(rigidbody.acceleration.y, allocator);
				rigidbodyComponent.AddMember("Acceleration", acceleration, allocator);

				Value force(rapidjson::kArrayType);
				force.PushBack(rigidbody.force.x, allocator).PushBack(rigidbody.force.y, allocator);
				rigidbodyComponent.AddMember("Force", force, allocator);

				// Add the remaining properties like Acceleration, Force, Mass, etc.
				rigidbodyComponent.AddMember("Mass", rigidbody.mass, allocator);
				rigidbodyComponent.AddMember("Inverse Mass", rigidbody.inverse_mass, allocator);
				rigidbodyComponent.AddMember("Linear Drag", rigidbody.linear_drag, allocator);
				
				rigidbodyComponent.AddMember("Angle", rigidbody.angle, allocator);
				rigidbodyComponent.AddMember("Angular Velocity", rigidbody.angular_velocity, allocator);
				rigidbodyComponent.AddMember("Angular Acceleration", rigidbody.angular_acceleration, allocator);
				
				rigidbodyComponent.AddMember("Torque", rigidbody.torque, allocator);
				rigidbodyComponent.AddMember("Inertia Mass", rigidbody.inertia_mass, allocator);
				rigidbodyComponent.AddMember("Inverse Inertia Mass", rigidbody.inv_inertia_mass? rigidbody.inv_inertia_mass:1, allocator);
				rigidbodyComponent.AddMember("Angular Drag", rigidbody.angular_drag, allocator);


				rigidbodyComponent.AddMember("use_gravity", rigidbody.use_gravity, allocator);
				rigidbodyComponent.AddMember("is_kinematic", rigidbody.is_kinematic, allocator);

				componentsArray.PushBack(rigidbodyComponent, allocator);
			}

			if (gameobject->HasComponent<BoxCollider2D>())
			{
				Value boxColliderComponent(rapidjson::kObjectType);
				boxColliderComponent.AddMember("Name", "BoxCollider2D", allocator);

				const auto& boxCollider = gameobject->GetComponent<BoxCollider2D>();
				Value min(rapidjson::kArrayType);
				min.PushBack(boxCollider.min.x, allocator).PushBack(boxCollider.min.y, allocator);
				boxColliderComponent.AddMember("Min", min, allocator);

				Value max(rapidjson::kArrayType);
				max.PushBack(boxCollider.max.x, allocator).PushBack(boxCollider.max.y, allocator);
				boxColliderComponent.AddMember("Max", max, allocator);

				boxColliderComponent.AddMember("Collision Flag", boxCollider.collision_flag, allocator);

				boxColliderComponent.AddMember("is_trigger", boxCollider.is_trigger, allocator);
				boxColliderComponent.AddMember("Tag", Value(boxCollider.tag.c_str(), allocator), allocator);

				componentsArray.PushBack(boxColliderComponent, allocator);
			}

			if (gameobject->HasComponent<CircleCollider2D>())
			{
				Value circleColliderComponent(rapidjson::kObjectType);
				circleColliderComponent.AddMember("Name", "CircleCollider2D", allocator);

				const auto& circleCollider = gameobject->GetComponent<CircleCollider2D>();
				Value center(rapidjson::kArrayType);
				center.PushBack(circleCollider.m_center.x, allocator).PushBack(circleCollider.m_center.y, allocator);
				circleColliderComponent.AddMember("Center", center, allocator);

				circleColliderComponent.AddMember("Radius", circleCollider.m_radius, allocator);

				componentsArray.PushBack(circleColliderComponent, allocator);
			}

			if (gameobject->HasComponent<SpriteRender>())
			{
				Value spriteRenderComponent(rapidjson::kObjectType);
				spriteRenderComponent.AddMember("Name", "SpriteRender", allocator);

				const auto& spriteRender = gameobject->GetComponent<SpriteRender>();
				spriteRenderComponent.AddMember("Sprite", Value(spriteRender.texturePath.c_str(), allocator), allocator);
				spriteRenderComponent.AddMember("Shape", spriteRender.shape == SPRITE_SHAPE::BOX ? 0 : 1, allocator);

				componentsArray.PushBack(spriteRenderComponent, allocator);
			}

			gameObjectData.AddMember("Components", componentsArray, allocator);


			gameObjectsArray.PushBack(gameObjectData, allocator);
		}
		// Add the array to the document
		document.AddMember("GameObjects", gameObjectsArray, allocator);

		std::string file = "../Assets/Scenes/" + file_name + ".json";
		if (!Serialization::PushJSON(file, document))
		{
			std::cerr << "Failed to save scene to file: " << file<< std::endl;
		}
	}
}