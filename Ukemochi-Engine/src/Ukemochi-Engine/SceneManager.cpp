/* Start Header ************************************************************************/
/*!
\file       SceneManager.cpp
\author     Tan Si Han, t.sihan, 2301264, t.sihan\@digipen.edu
\date       Nov 8, 2024
\brief      This file contains the definition of the SceneManger system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "SceneManager.h"
#include "Audio/Audio.h"
#include "Physics/Physics.h"
#include "Collision/Collision.h"
#include "Math/Transformation.h"
#include "Graphics/Renderer.h"
#include "Serialization/Serialization.h"
#include "Logic/Logic.h"
#include "Graphics/Camera2D.h"
#include "Factory/Factory.h"
#include "Factory/GameObjectManager.h"
#include "ImGui/ImGuiCore.h"
#include "InGameGUI/InGameGUI.h"
#include "Application.h"
#include "Game/PlayerManager.h"
#include "Graphics/Animation.h"
#include "Game/EnemyManager.h"
#include "Game/DungeonManager.h"

namespace Ukemochi
{
    //for save & load
    using namespace rapidjson;

    const float SPRITE_SCALE = 100.f;
    const float ENTITY_ACCEL = 150.f;
    const float PLAYER_FORCE = 1500.f;

    std::chrono::duration<double> SceneManager::loop_time{};
    std::chrono::duration<double> SceneManager::collision_time{};
    std::chrono::duration<double> SceneManager::physics_time{};
    std::chrono::duration<double> SceneManager::graphics_time{};
    std::chrono::duration<double> SceneManager::logic_time{};


	//bool func_toggle = false;

	SceneManager::SceneManager()
		:sceneName("NewScene"), cameraSize(0,0)
	{
		es_current = ES_ENGINE;
		// Set up ECS
		ECS::GetInstance().Init();

        // TODO: Register your components, the limit is 32 components
        ECS::GetInstance().RegisterComponent<Transform>();
        ECS::GetInstance().RegisterComponent<Rigidbody2D>();
        ECS::GetInstance().RegisterComponent<BoxCollider2D>();
        ECS::GetInstance().RegisterComponent<CircleCollider2D>();
        ECS::GetInstance().RegisterComponent<ConvexCollider2D>();
        ECS::GetInstance().RegisterComponent<SpriteRender>();
	    ECS::GetInstance().RegisterComponent<Animation>();
        ECS::GetInstance().RegisterComponent<Script>();
        ECS::GetInstance().RegisterComponent<Player>();
        ECS::GetInstance().RegisterComponent<Enemy>();
        ECS::GetInstance().RegisterComponent<AudioManager>();

        // TODO: Register your systems, No limit for systems
        ECS::GetInstance().RegisterSystem<Physics>();
        ECS::GetInstance().RegisterSystem<Collision>();
        ECS::GetInstance().RegisterSystem<Transformation>();
        ECS::GetInstance().RegisterSystem<Renderer>();
        ECS::GetInstance().RegisterSystem<LogicSystem>();
        ECS::GetInstance().RegisterSystem<Camera>();
        ECS::GetInstance().RegisterSystem<InGameGUI>();
        ECS::GetInstance().RegisterSystem<Audio>();
		ECS::GetInstance().RegisterSystem<AssetManager>();
	    ECS::GetInstance().RegisterSystem<AnimationSystem>();
        ECS::GetInstance().RegisterSystem<EnemyManager>();

		ECS::GetInstance().RegisterSystem<PlayerManager>();
        ECS::GetInstance().RegisterSystem<DungeonManager>();

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

        // For in game GUI system
        sig.reset();
        sig.set(ECS::GetInstance().GetComponentType<Transform>());
        ECS::GetInstance().SetSystemSignature<InGameGUI>(sig);

	    // For Animation System
	    sig.reset();
	    sig.set(ECS::GetInstance().GetComponentType<Animation>());
	    ECS::GetInstance().SetSystemSignature<AnimationSystem>(sig);

		// For Player system
		sig.reset();
		sig.set(ECS::GetInstance().GetComponentType<Player>());
		ECS::GetInstance().SetSystemSignature<PlayerManager>(sig);

        //For Enemy
        sig.reset();
        sig.set(ECS::GetInstance().GetComponentType<Enemy>());
        ECS::GetInstance().SetSystemSignature<EnemyManager>(sig);

        // For Player system
        sig.reset();
        sig.set(ECS::GetInstance().GetComponentType<AudioManager>());
        ECS::GetInstance().SetSystemSignature<Audio>(sig);

        //init GSM
        //GSM_Initialize(GS_ENGINE);
    }

    SceneManager::~SceneManager()
    {
    }

    void SceneManager::SceneMangerLoad()
    {
        //load all assest
		UME_ENGINE_TRACE("Loading Assets...");
        //ECS::GetInstance().GetSystem<Audio>()->GetInstance().LoadSound(R"(../Assets/Audio/BGM_game.mp3)");
        //ECS::GetInstance().GetSystem<Audio>()->GetInstance().LoadSound(R"(../Assets/Audio/SFX_jump.wav)");
        //ECS::GetInstance().GetSystem<Audio>()->GetInstance().LoadSound(R"(../Assets/Audio/UI_button_confirm.wav)");
        //ECS::GetInstance().GetSystem<Audio>()->GetInstance().LoadSound(R"(../Assets/Audio/SFX_knight_ready.ogg)");

        //load Asset Manager Texture
		ECS::GetInstance().GetSystem<AssetManager>()->addTexture("../Assets/Textures/Moon Floor.png", ECS::GetInstance().GetSystem<AssetManager>()->order_index);
		ECS::GetInstance().GetSystem<AssetManager>()->addTexture("../Assets/Textures/Worm.png", ECS::GetInstance().GetSystem<AssetManager>()->order_index);
		ECS::GetInstance().GetSystem<AssetManager>()->addTexture("../Assets/Textures/running_player_sprite_sheet.png", ECS::GetInstance().GetSystem<AssetManager>()->order_index);
		ECS::GetInstance().GetSystem<AssetManager>()->addTexture("../Assets/Textures/idle_player_sprite_sheet.png", ECS::GetInstance().GetSystem<AssetManager>()->order_index);
		ECS::GetInstance().GetSystem<AssetManager>()->addTexture("../Assets/Textures/Mochi_Attack_1.png", ECS::GetInstance().GetSystem<AssetManager>()->order_index);
		ECS::GetInstance().GetSystem<AssetManager>()->addTexture("../Assets/Textures/Mochi_Attack_2.png", ECS::GetInstance().GetSystem<AssetManager>()->order_index);
		ECS::GetInstance().GetSystem<AssetManager>()->addTexture("../Assets/Textures/Mochi_Attack_3.png", ECS::GetInstance().GetSystem<AssetManager>()->order_index);
		ECS::GetInstance().GetSystem<AssetManager>()->addTexture("../Assets/Textures/Mochi_Death_SS.png", ECS::GetInstance().GetSystem<AssetManager>()->order_index);
		ECS::GetInstance().GetSystem<AssetManager>()->addTexture("../Assets/Textures/Mochi_Hurt_SS.png", ECS::GetInstance().GetSystem<AssetManager>()->order_index);
		
        // Load UI textures
        ECS::GetInstance().GetSystem<AssetManager>()->addTexture("../Assets/Textures/UI/ui_game.png", ECS::GetInstance().GetSystem<AssetManager>()->order_index);

        //Get Scenelist
		UME_ENGINE_TRACE("Loading Scenes...");
        UseImGui::LoadScene();

        //if fresh start
        if (UseImGui::GetSceneSize() == 0)
        {
            CreateNewScene("NewScene");
        }
        else
        {
            //load first scene
            LoadSaveFile(UseImGui::GetStartScene());
        }

        Application& app = Application::Get();
        int screen_width = app.GetWindow().GetWidth();
        int screen_height = app.GetWindow().GetHeight();

        ECS::GetInstance().GetSystem<InGameGUI>()->CreateText("text1", "pls click a button",
            Vec2{ screen_width * 0.1f, screen_height * 0.9f },
            1.f, Vec3{ 1.f, 1.f, 1.f }, "Ukemochi");

        ECS::GetInstance().GetSystem<InGameGUI>()->CreateText("text2", "Hi",
            Vec2{ screen_width * 0.5f, screen_height * 0.9f },
            1.f, Vec3{ 1.f, 1.f, 1.f }, "Exo2");

        //UME_ENGINE_TRACE("Initializing Collision...");
        //ECS::GetInstance().GetSystem<Collision>()->Init();
        //UME_ENGINE_TRACE("Initializing dungeon manager...");
        //ECS::GetInstance().GetSystem<DungeonManager>()->Init();
    }

    void SceneManager::SceneMangerInit()
    {
        // Initialize the graphics and collision system
		UME_ENGINE_TRACE("Setting up shaders...");
        ECS::GetInstance().GetSystem<Renderer>()->setUpShaders();
        ECS::GetInstance().GetSystem<AssetManager>()->addShader("default", "../Assets/Shaders/default.vert", "../Assets/Shaders/default.frag");

		UME_ENGINE_TRACE("Initializing renderer...");
        ECS::GetInstance().GetSystem<Renderer>()->init();
        UME_ENGINE_TRACE("Initializing in game GUI...");
        ECS::GetInstance().GetSystem<InGameGUI>()->Init();
    }

     //When in game Engine State
    void SceneManager::SceneMangerUpdate()
    {
        ECS::GetInstance().GetSystem<Transformation>()->ComputeTransformations();

        ECS::GetInstance().GetSystem<Audio>()->GetInstance().Update();

        SceneManagerDraw();

        if (Input::IsKeyTriggered(UME_KEY_U))
            ECS::GetInstance().GetSystem<Renderer>()->debug_mode_enabled = static_cast<GLboolean>(!ECS::GetInstance().
                GetSystem<Renderer>()->debug_mode_enabled);

        // On mouse button press, start dragging
        if (Input::IsMouseButtonTriggered(GLFW_MOUSE_BUTTON_LEFT))
        {
            ECS::GetInstance().GetSystem<Renderer>()->handleMouseClickOP(
                static_cast<int>(SceneManager::GetInstance().GetPlayScreen().x + ECS::GetInstance().GetSystem<Camera>()->position.x),
                static_cast<int>(SceneManager::GetInstance().GetPlayScreen().y + ECS::GetInstance().GetSystem<Camera>()->position.y));
        }

        // On mouse movement, continue dragging if active
        if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
        {
            ECS::GetInstance().GetSystem<Renderer>()->handleMouseDrag(
                static_cast<int>(SceneManager::GetInstance().GetPlayScreen().x + ECS::GetInstance().GetSystem<Camera>()->position.x),
                static_cast<int>(SceneManager::GetInstance().GetPlayScreen().y + ECS::GetInstance().GetSystem<Camera>()->position.y));
        }

        // On mouse button release, stop dragging
        if (Input::IsMouseButtonTriggered(GLFW_MOUSE_BUTTON_RIGHT)) // Or GLFW_RELEASE
        {
            ECS::GetInstance().GetSystem<Renderer>()->isDragging = false;
            ECS::GetInstance().GetSystem<Renderer>()->selectedEntityID = -1; // Clear selection
        }

    }

    //When engine is Running the scene
    void SceneManager::SceneMangerRunSystems()
    {
        loop_start = std::chrono::steady_clock::now();

        // Debug mode
        if (Input::IsKeyTriggered(UME_KEY_U))
            ECS::GetInstance().GetSystem<Renderer>()->debug_mode_enabled = static_cast<GLboolean>(!ECS::GetInstance().
                GetSystem<Renderer>()->debug_mode_enabled);

        // Select entity
        if (Input::IsMouseButtonTriggered(GLFW_MOUSE_BUTTON_LEFT))
        {
            ECS::GetInstance().GetSystem<Renderer>()->handleMouseClickOP(SceneManager::GetInstance().GetPlayScreen().x + ECS::GetInstance().GetSystem<Camera>()->position.x,
                SceneManager::GetInstance().GetPlayScreen().y + ECS::GetInstance().GetSystem<Camera>()->position.y);
        }
        /*
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
        */
        if (Ukemochi::Input::IsKeyPressed(GLFW_KEY_M))
        {
            ECS::GetInstance().GetSystem<Audio>()->GetInstance().StopAllSoundsInGroup(LEVEL1);
        }
        if (Ukemochi::Input::IsKeyPressed(GLFW_KEY_N))
        {
            ECS::GetInstance().GetSystem<Audio>()->GetInstance().PlayAllSoundsInGroup(LEVEL1);
        }

		/*if (Ukemochi::Input::IsKeyTriggered(GLFW_KEY_1)&&!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsPlaying(BGM))
		{
			ECS::GetInstance().GetSystem<Audio>()->GetInstance().PlaySoundInGroup(AudioList::BGM, ChannelGroups::LEVEL1);
			ECS::GetInstance().GetSystem<Audio>()->GetInstance().SetAudioVolume(BGM, 0.04f);
		}
		if (Ukemochi::Input::IsKeyTriggered(GLFW_KEY_2) && !ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsPlaying(HIT))
		{
			ECS::GetInstance().GetSystem<Audio>()->GetInstance().PlaySoundInGroup(AudioList::HIT, ChannelGroups::LEVEL1);
			ECS::GetInstance().GetSystem<Audio>()->GetInstance().SetAudioVolume(HIT, 0.04f);
		}
		if (Ukemochi::Input::IsKeyTriggered(GLFW_KEY_3) && !ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsPlaying(READY))
		{
			ECS::GetInstance().GetSystem<Audio>()->GetInstance().PlaySoundInGroup(AudioList::READY, ChannelGroups::LEVEL1);
			ECS::GetInstance().GetSystem<Audio>()->GetInstance().SetAudioVolume(READY, 0.04f);
		}
		if (Ukemochi::Input::IsKeyTriggered(GLFW_KEY_4) && !ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsPlaying(CONFIRMCLICK))
		{
			ECS::GetInstance().GetSystem<Audio>()->GetInstance().PlaySoundInGroup(AudioList::CONFIRMCLICK, ChannelGroups::LEVEL1);
			ECS::GetInstance().GetSystem<Audio>()->GetInstance().SetAudioVolume(CONFIRMCLICK, 0.04f);
		}*/

        ECS::GetInstance().GetSystem<EnemyManager>()->UpdateEnemies();

		ECS::GetInstance().GetSystem<Audio>()->GetInstance().Update();

        ECS::GetInstance().GetSystem<InGameGUI>()->Update();
        // ECS::GetInstance().GetSystem<Renderer>()->animationKeyInput();

        // --- GAME LOGIC UPDATE ---
	    sys_start = std::chrono::steady_clock::now();
        ECS::GetInstance().GetSystem<LogicSystem>()->Update();
		ECS::GetInstance().GetSystem<PlayerManager>()->Update();
	    sys_end = std::chrono::steady_clock::now();
	    logic_time = std::chrono::duration_cast<std::chrono::duration<double>>(sys_end - sys_start);
	    
        // --- PHYSICS UPDATE ---
        // Update the entities physics
        sys_start = std::chrono::steady_clock::now();
		ECS::GetInstance().GetSystem<Physics>()->UpdatePhysics();
		sys_end = std::chrono::steady_clock::now();
		physics_time = std::chrono::duration_cast<std::chrono::duration<double>>(sys_end - sys_start);

        // --- COLLISION UPDATE ---
        // Check the collisions between the entities
        sys_start = std::chrono::steady_clock::now();
		ECS::GetInstance().GetSystem<Collision>()->CheckCollisions();
		sys_end = std::chrono::steady_clock::now();
		collision_time = std::chrono::duration_cast<std::chrono::duration<double>>(sys_end - sys_start);

        // --- TRANSFORMATION UPDATE ---
        // Compute the entities transformations
        ECS::GetInstance().GetSystem<Transformation>()->ComputeTransformations();

	    // --- ANIMATION UPDATE ---
	    ECS::GetInstance().GetSystem<AnimationSystem>()->Update();

	    // --- RENDERER UPDATE ---
        sys_start = std::chrono::steady_clock::now();
		SceneManagerDraw();
		sys_end = std::chrono::steady_clock::now();
		graphics_time = std::chrono::duration_cast<std::chrono::duration<double>>(sys_end - sys_start);

		loop_end = std::chrono::steady_clock::now();
		loop_time = std::chrono::duration_cast<std::chrono::duration<double>>(loop_end - loop_start);
	}

    void SceneManager::SceneMangerUpdateCamera(double deltaTime)
    {
        // Camera
        ECS::GetInstance().GetSystem<Camera>()->processCameraInput(static_cast<GLfloat>(deltaTime));
    }

    void SceneManager::SceneManagerDraw()
    {
#ifdef _DEBUG
        ECS::GetInstance().GetSystem<Renderer>()->renderToFramebuffer();
        //ECS::GetInstance().GetSystem<Renderer>()->renderForObjectPicking();
#endif // _DEBUG

#ifndef _DEBUG
        ECS::GetInstance().GetSystem<Renderer>()->render();
#endif // !_DEBUG
    }

    void SceneManager::SceneManagerFree()
    {
		UME_ENGINE_TRACE("Destroying all game objects...");
        std::vector<GameObject*> list = GameObjectManager::GetInstance().GetAllGOs();
        for (auto& gameobject : list)
        {
            GameObjectManager::GetInstance().DestroyObject(gameobject->GetInstanceID());
        }
        ECS::GetInstance().GetSystem<LogicSystem>()->End();
		UME_ENGINE_TRACE("Resetting entity manager...");
        ECS::GetInstance().ReloadEntityManager();
        ECS::GetInstance().GetSystem<Audio>()->GetInstance().StopAudioGroup(ChannelGroups::LEVEL1);
    }

    void SceneManager::SceneManagerUnload()
    {
        SceneManagerFree();
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

    void SceneManager::LoadSaveFile(const std::string& file_name)
    {
        std::string sceneCpy = file_name;
        GetInstance().sceneName = file_name;
        size_t extensionPos = sceneCpy.find(".json");
        if (extensionPos != std::string::npos)
        {
            GetInstance().sceneName = sceneCpy.substr(0, extensionPos);
        }
        //unload curr scene
        SceneManagerFree();
        bool playerFound = false;

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

            //if (tag == "Button")
            //{
            //    // Get the screen width and height
            //    Application& app = Application::Get();
            //    int screen_width = app.GetWindow().GetWidth();
            //    int screen_height = app.GetWindow().GetHeight();

            //    ECS::GetInstance().GetSystem<InGameGUI>()->CreateText("text1", "pls click a button",
            //                                                          Vec2{screen_width * 0.1f, screen_height * 0.9f},
            //                                                          1.f, Vec3{1.f, 1.f, 1.f}, "Ukemochi");

            //    Button buttonComponent;
            //    buttonComponent.on_click = []()
            //    {
            //        std::cout << "PRESSED" << std::endl;
            //        ECS::GetInstance().GetSystem<InGameGUI>()->UpdateText("text1", "pause button clicked!");
            //    };
            //    ECS::GetInstance().GetSystem<InGameGUI>()->CreateButtonOBJ(
            //        name, tag, "pause_btn", "", Vec2{screen_width * 0.05f, screen_height * 0.8f}, 1.f,
            //        Vec3{1.f, 1.f, 1.f}, "Ukemochi",
            //        Vec2{75.f, 75.f}, "../Assets/Textures/UI/pause.png", buttonComponent.on_click);
            //    continue;
            //    //newObject.AddComponent(buttonComponent);
            //    //newObject.AddComponent(Button{ [this]() { ECS::GetInstance().GetSystem<InGameGUI>()->UpdateText("text1", "pause button clicked!"); } });
            //    //ECS::GetInstance().GetSystem<InGameGUI>()->CreateButtonOBJ(newObject, "pause_btn", "", Vec2{ screen_width * 0.05f, screen_height * 0.8f }, 1.f, Vec3{ 1.f, 1.f, 1.f }, "Ukemochi",
            //    //Vec2{ 75.f, 75.f }, "../Assets/Textures/UI/pause.png", [newObject]() { ECS::GetInstance().GetSystem<InGameGUI>()->UpdateText("text1", "pause button clicked!"); });
            //}

            // Create a new GameObject and add it to the scene

            GameObject& newObject = GameObjectManager::GetInstance().CreateObject(name, tag);
            //GameObject newObject = GameObject(entity,name, tag);

        	// TODO: Encapsulate this into factory during refactoring
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
                    float rotation = componentData["Rotation"].GetFloat();
                    Vec2 scale(
                        componentData["Scale"][0].GetFloat(),
                        componentData["Scale"][1].GetFloat()
                    );

                    if (!newObject.HasComponent<Transform>())
                    {
                        newObject.AddComponent<Transform>({Mtx44(), position, rotation, scale});
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
                        newObject.AddComponent<Rigidbody2D>({
                            position, velocity, acceleration, force, mass, inverseMass, linearDrag,
                            angle, angularVelocity, angularAcceleration, torque, inertiaMass,
                            inverseInertiaMass, angularDrag, useGravity, isKinematic
                        });
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
                    //std::string collisionTag = componentData["Tag"].GetString();
                    if (!newObject.HasComponent<BoxCollider2D>())
                    {
                        newObject.AddComponent<BoxCollider2D>({min, max, collisionFlag, isTrigger});
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
                        newObject.AddComponent<CircleCollider2D>({center, radius});
                    }
                }
                else if (componentName == "SpriteRender")
                {
                    std::string texturePath = componentData["Sprite"].GetString();
                    SPRITE_SHAPE shape = componentData["Shape"].GetInt() == 0
                                             ? SPRITE_SHAPE::BOX
                                             : SPRITE_SHAPE::CIRCLE;
                    if (!newObject.HasComponent<SpriteRender>())
                    {
                        SpriteRender sr = {texturePath, shape};
                        newObject.AddComponent<SpriteRender>(sr);
                    }

                    ECS::GetInstance().GetSystem<AssetManager>()->addTexture(newObject.GetComponent<SpriteRender>().texturePath, ECS::GetInstance().GetSystem<AssetManager>()->order_index);
					// if (tag == "Player")
					// {
					// 	newObject.GetComponent<SpriteRender>().animated = true;
					// 	newObject.GetComponent<SpriteRender>().animationIndex = 1;
					// }
				}
				else if (componentName == "Script")
				{
					if(!newObject.HasComponent<Script>())
						{
                        MonoObject* newScript = ScriptingEngine::GetInstance().InstantiateClientClass(
                            componentData["ClassName"].GetString()); 
                        EntityID newScriptID = newObject.GetInstanceID();
                        ScriptingEngine::SetMonoFieldValueULL(newScript, "_id", &newScriptID);
                        newObject.AddComponent(Script{
                            componentData["Path"].GetString(),
                            componentData["ClassName"].GetString(),
                            newScript,
                            ScriptingEngine::CreateGCHandle(newScript)
                        });
                    }
                }
            	else if (componentName == "Animation")
            	{
            		if(!newObject.HasComponent<Animation>())
            		{
            			// Get clips and put them in map of Animation
            			Animation anim;
            			
            			for (auto itr = componentData["Clips"].MemberBegin(); itr != componentData["Clips"].MemberEnd(); ++itr)
						{
							AnimationClip newClip;
            				newClip.keyPath = itr->value[0].GetString();
							newClip.name = itr->value[1].GetString();
							newClip.total_frames = itr->value[2].GetInt();
            				newClip.pivot.x = itr->value[3].GetFloat();
            				newClip.pivot.y = itr->value[4].GetFloat();
            				newClip.pixelsPerUnit = itr->value[5].GetInt();
							newClip.pixel_width = itr->value[6].GetInt();
							newClip.pixel_height = itr->value[7].GetInt();
							newClip.total_width = itr->value[8].GetInt();
							newClip.total_height = itr->value[9].GetInt();
							newClip.frame_time = itr->value[10].GetFloat();
							newClip.looping = itr->value[11].GetBool();
							anim.clips[newClip.name] = newClip;
						}

            			anim.currentClip = componentData["CurrentClip"].GetString();

            			newObject.AddComponent(std::move(anim));
            		}
            	}
                else if (componentName == "EnemyComponent")
                {
                    if (!newObject.HasComponent<Enemy>())
                    {
                        // Deserialize Enemy component
                        int type = componentData["Type"].GetInt();

                        newObject.AddComponent<Enemy>({ componentData["Position"][0].GetFloat(),
                            componentData["Position"][1].GetFloat(),static_cast<Enemy::EnemyTypes>(type),newObject.GetInstanceID() });
                    }
                }
            	else if (componentName == "Player")
            	{
            		if (!newObject.HasComponent<Player>())
            		{
            			Player player;
            			player.maxHealth = componentData["MaxHealth"].GetInt();
            			player.currentHealth = componentData["CurrentHealth"].GetInt();
            			player.maxComboHits = componentData["MaxComboHits"].GetInt();
            			player.currentComboHits = componentData["CurrentComboHits"].GetInt();
            			player.comboDamage = componentData["ComboDamage"].GetInt();
            			player.attackCooldown = componentData["AttackCooldown"].GetFloat();
            			player.attackTimer = componentData["AttackTimer"].GetFloat();
            			player.playerForce = componentData["PlayerForce"].GetFloat();
            			player.isDead = componentData["IsDead"].GetBool();
            			player.canAttack = componentData["CanAttack"].GetBool();
            			// TODO: is there a better way to do this? type.id()?

            			newObject.AddComponent(std::move(player));
            		}
            	}
                else if (componentName == "Audio")
                {
                    if (!newObject.HasComponent<AudioManager>())
                    {
                        AudioManager audio;

                        // Load Music List
                        const auto& musicArray = componentData["Music"].GetArray();
                        for (const auto& musicObject : musicArray)
                        {
                            std::string musicname = musicObject["Name"].GetString();
                            std::string path = musicObject["Path"].GetString();
                            audio.music.emplace_back(musicname, path);
                            audio.AddSoundToMusic(musicname);
                        }

                        // Load SFX List
                        const auto& sfxArray = componentData["SFX"].GetArray();
                        for (const auto& sfxObject : sfxArray)
                        {
                            std::string sfxname = sfxObject["Name"].GetString();
                            std::string path = sfxObject["Path"].GetString();
                            audio.sfx.emplace_back(sfxname, path);
                            audio.AddSoundToSfx(sfxname);
                        }

                        // Add the AudioManager component to the object
                        newObject.AddComponent(std::move(audio));
                    }
                    }
                else
                {
                    UME_ENGINE_ERROR("Unknown component type: {0}", componentName);
                }

                if (tag == "Player")
                {
                    playerFound = true;

                    ECS::GetInstance().GetSystem<Renderer>()->SetPlayer(static_cast<int>(newObject.GetInstanceID()));
                    //ECS::GetInstance().GetSystem<Renderer>()->SetPlayerObject(newObject);
                    // ECS::GetInstance().GetSystem<Renderer>()->initAnimationEntities();
                }
                if (tag == "Enemy")
                {

                }
                if (!playerFound)
                    ECS::GetInstance().GetSystem<Renderer>()->SetPlayer(-1);
            }
        }

        // std::cout << "Scene loaded successfully from file: " << file_path << std::endl;
        UME_ENGINE_INFO("Scene loaded successfully from file: {0}", file_path);
    }

    void SceneManager::SaveScene(const std::string& file_name)
    {
        //get file name to save
        Document document;
        document.SetObject();
        Document::AllocatorType& allocator = document.GetAllocator();

        // Create a JSON array to hold game object data
        Value gameObjectsArray(rapidjson::kArrayType);

		// TODO: Should Encapsulate this into factory? or Serialization? 
        std::vector<GameObject*> list = GameObjectManager::GetInstance().GetAllGOs();
        for (auto& gameobject : list)
        {
            //set the type
            Value gameObjectData(rapidjson::kObjectType);
            gameObjectData.AddMember("Name", Value(gameobject->GetName().c_str(), allocator), allocator);
            gameObjectData.AddMember("Tag", Value(gameobject->GetTag().c_str(), allocator), allocator);

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

                transformComponent.AddMember("Rotation", transform.rotation, allocator);

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
                acceleration.PushBack(rigidbody.acceleration.x, allocator).
                             PushBack(rigidbody.acceleration.y, allocator);
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
                rigidbodyComponent.AddMember("Inverse Inertia Mass",
                                             rigidbody.inv_inertia_mass ? rigidbody.inv_inertia_mass : 1, allocator);
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
                //boxColliderComponent.AddMember("Tag", Value(boxCollider.tag.c_str(), allocator), allocator);

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
                spriteRenderComponent.AddMember("Sprite", Value(spriteRender.texturePath.c_str(), allocator),
                                                allocator);
                spriteRenderComponent.AddMember("Shape", spriteRender.shape == SPRITE_SHAPE::BOX ? 0 : 1, allocator);

                componentsArray.PushBack(spriteRenderComponent, allocator);
            }

            if (gameobject->HasComponent<Script>())
            {
                Value scriptComponent(rapidjson::kObjectType);
                scriptComponent.AddMember("Name", "Script", allocator);

                const auto& script = gameobject->GetComponent<Script>();
                scriptComponent.AddMember("Path", Value(script.scriptPath.c_str(), allocator), allocator);
                scriptComponent.AddMember("ClassName", Value(script.scriptName.c_str(), allocator), allocator);

                componentsArray.PushBack(scriptComponent, allocator);
            }

        	if(gameobject->HasComponent<Animation>())
        	{
        		Value animationComponent(rapidjson::kObjectType);
        		animationComponent.AddMember("Name", "Animation", allocator);

        		Value clipsList(kObjectType);
        		const auto& animation = gameobject->GetComponent<Animation>();
        		for(const auto& [key,value] : animation.clips)
        		{
        			Value clip(key.c_str(),allocator);
        			Value clipData(kArrayType);
        			clipData.PushBack(Value(value.keyPath.c_str(),allocator), allocator);
        			clipData.PushBack(Value(key.c_str(),allocator), allocator);

        			clipData.PushBack(value.total_frames, allocator);
        			clipData.PushBack(value.pivot.x, allocator);
        			clipData.PushBack(value.pivot.y, allocator);
        			clipData.PushBack(value.pixelsPerUnit, allocator);
        			clipData.PushBack(value.pixel_width, allocator);
        			clipData.PushBack(value.pixel_height,allocator);
        			clipData.PushBack(value.total_width, allocator);
        			clipData.PushBack(value.total_height, allocator);
        			clipData.PushBack(value.frame_time, allocator);
        			clipData.PushBack(value.looping, allocator);
        			clipsList.AddMember(clip, clipData, allocator);
        		}
        		animationComponent.AddMember("Clips", clipsList, allocator);
        		animationComponent.AddMember("CurrentClip", Value(animation.currentClip.c_str(), allocator), allocator);
        		componentsArray.PushBack(animationComponent, allocator);
        	}
            if (gameobject->HasComponent<Enemy>())
            {
                Value enemyComponent(rapidjson::kObjectType);

                enemyComponent.AddMember("Name", Value("EnemyComponent", allocator), allocator);

                const auto& enemy = gameobject->GetComponent<Enemy>();

                Value position(rapidjson::kArrayType);
                position.PushBack(enemy.posX, allocator);
                position.PushBack(enemy.posY, allocator);
                enemyComponent.AddMember("Position", position, allocator);

                enemyComponent.AddMember("Type", enemy.type, allocator);

                componentsArray.PushBack(enemyComponent, allocator);
            }

        	if (gameobject->HasComponent<Player>())
        	{
        		Value playerComponent(rapidjson::kObjectType);
        		playerComponent.AddMember("Name", "Player", allocator);

        		const auto&player = gameobject->GetComponent<Player>();
        		playerComponent.AddMember("MaxHealth", player.maxHealth, allocator);
        		playerComponent.AddMember("CurrentHealth", player.currentHealth, allocator);
        		playerComponent.AddMember("MaxComboHits", player.maxComboHits, allocator);
        		playerComponent.AddMember("CurrentComboHits", player.currentComboHits, allocator);
        		playerComponent.AddMember("ComboDamage", player.comboDamage, allocator);
        		playerComponent.AddMember("AttackCooldown", player.attackCooldown, allocator);
        		playerComponent.AddMember("AttackTimer", player.attackTimer, allocator);
        		playerComponent.AddMember("PlayerForce", player.playerForce, allocator);
        		playerComponent.AddMember("IsDead", player.isDead, allocator);
        		playerComponent.AddMember("CanAttack", player.canAttack, allocator);

        		componentsArray.PushBack(playerComponent, allocator);
        	}
            if (gameobject->HasComponent<AudioManager>()) {
                Value audioComponent(rapidjson::kObjectType);
                audioComponent.AddMember("Name", "Audio", allocator);

                const auto& audioManager = gameobject->GetComponent<AudioManager>();

                // Save Music List
                Value musicArray(rapidjson::kArrayType);
                for (const auto& music : audioManager.music) {
                    Value musicObject(rapidjson::kObjectType);

                    // Add name and path for each music entry
                    musicObject.AddMember("Name", Value(music.audioName.c_str(), allocator), allocator);
                    musicObject.AddMember("Path", Value(music.audioPath.c_str(), allocator), allocator);

                    // Add to the music array
                    musicArray.PushBack(musicObject, allocator);
                }
                audioComponent.AddMember("Music", musicArray, allocator);

                // Save SFX List
                Value sfxArray(rapidjson::kArrayType);
                for (const auto& sfx : audioManager.sfx) {
                    Value sfxObject(rapidjson::kObjectType);

                    // Add name and path for each SFX entry
                    sfxObject.AddMember("Name", Value(sfx.audioName.c_str(), allocator), allocator);
                    sfxObject.AddMember("Path", Value(sfx.audioPath.c_str(), allocator), allocator);

                    // Add to the SFX array
                    sfxArray.PushBack(sfxObject, allocator);
                }
                audioComponent.AddMember("SFX", sfxArray, allocator);

                // Add the audio component to the components array
                componentsArray.PushBack(audioComponent, allocator);
            }

            gameObjectData.AddMember("Components", componentsArray, allocator);


            gameObjectsArray.PushBack(gameObjectData, allocator);
        }
        // Add the array to the document
        document.AddMember("GameObjects", gameObjectsArray, allocator);

        std::string file = "../Assets/Scenes/" + file_name + ".json";
        if (!Serialization::PushJSON(file, document))
        {
            std::cerr << "Failed to save scene to file: " << file << std::endl;
        }
    }

    void SceneManager::CreateNewScene(const std::string& file_name)
    {
        Document document;
        document.SetObject();
        Document::AllocatorType& allocator = document.GetAllocator();

        // Create a JSON array to hold game object data
        Value gameObjectsArray(rapidjson::kArrayType);
        document.AddMember("GameObjects", gameObjectsArray, allocator);

        std::string file = "../Assets/Scenes/" + file_name + ".json";
        if (!Serialization::PushJSON(file, document))
        {
            std::cerr << "Failed to save scene to file: " << file << std::endl;
        }
    }

    void SceneManager::SavePrefab(GameObject* prefabObj, const std::string& file_name)
    {
        // TODO: Some day we will encapsulate all this into Serialization class...
        //get file name to save
        Document document;
        document.SetObject();
        Document::AllocatorType& allocator = document.GetAllocator();

        // Create a JSON array to hold game object data
        //Value gameObjectsArray(rapidjson::kObjectType);

        //set the type
        Value gameObjectData(rapidjson::kObjectType);
        gameObjectData.AddMember("Name", Value(prefabObj->GetName().c_str(), allocator), allocator);
        gameObjectData.AddMember("Tag", Value(prefabObj->GetTag().c_str(), allocator), allocator);

        //set the type
        Value componentsArray(rapidjson::kArrayType);

        if (prefabObj->HasComponent<Transform>())
        {
            Value transformComponent(rapidjson::kObjectType);

            transformComponent.AddMember("Name", Value("Transform", allocator), allocator);

            const auto& transform = prefabObj->GetComponent<Transform>();
            Value position(rapidjson::kArrayType);
            position.PushBack(transform.position.x, allocator);
            position.PushBack(transform.position.y, allocator);
            transformComponent.AddMember("Position", position, allocator);

            transformComponent.AddMember("Rotation", transform.rotation, allocator);

            Value scale(rapidjson::kArrayType);
            scale.PushBack(transform.scale.x, allocator);
            scale.PushBack(transform.scale.y, allocator);
            transformComponent.AddMember("Scale", scale, allocator);

            componentsArray.PushBack(transformComponent, allocator);
        }

        if (prefabObj->HasComponent<Rigidbody2D>())
        {
            Value rigidbodyComponent(rapidjson::kObjectType);
            rigidbodyComponent.AddMember("Name", "Rigidbody2D", allocator);

            const auto& rigidbody = prefabObj->GetComponent<Rigidbody2D>();
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
            rigidbodyComponent.AddMember("Inverse Inertia Mass",
                                         rigidbody.inv_inertia_mass ? rigidbody.inv_inertia_mass : 1, allocator);
            rigidbodyComponent.AddMember("Angular Drag", rigidbody.angular_drag, allocator);


            rigidbodyComponent.AddMember("use_gravity", rigidbody.use_gravity, allocator);
            rigidbodyComponent.AddMember("is_kinematic", rigidbody.is_kinematic, allocator);

            componentsArray.PushBack(rigidbodyComponent, allocator);
        }

        if (prefabObj->HasComponent<BoxCollider2D>())
        {
            Value boxColliderComponent(rapidjson::kObjectType);
            boxColliderComponent.AddMember("Name", "BoxCollider2D", allocator);

            const auto& boxCollider = prefabObj->GetComponent<BoxCollider2D>();
            Value min(rapidjson::kArrayType);
            min.PushBack(boxCollider.min.x, allocator).PushBack(boxCollider.min.y, allocator);
            boxColliderComponent.AddMember("Min", min, allocator);

            Value max(rapidjson::kArrayType);
            max.PushBack(boxCollider.max.x, allocator).PushBack(boxCollider.max.y, allocator);
            boxColliderComponent.AddMember("Max", max, allocator);

            boxColliderComponent.AddMember("Collision Flag", boxCollider.collision_flag, allocator);

            boxColliderComponent.AddMember("is_trigger", boxCollider.is_trigger, allocator);
            //boxColliderComponent.AddMember("Tag", Value(boxCollider.tag.c_str(), allocator), allocator);

            componentsArray.PushBack(boxColliderComponent, allocator);
        }

        if (prefabObj->HasComponent<CircleCollider2D>())
        {
            Value circleColliderComponent(rapidjson::kObjectType);
            circleColliderComponent.AddMember("Name", "CircleCollider2D", allocator);

            const auto& circleCollider = prefabObj->GetComponent<CircleCollider2D>();
            Value center(rapidjson::kArrayType);
            center.PushBack(circleCollider.m_center.x, allocator).PushBack(circleCollider.m_center.y, allocator);
            circleColliderComponent.AddMember("Center", center, allocator);

            circleColliderComponent.AddMember("Radius", circleCollider.m_radius, allocator);

            componentsArray.PushBack(circleColliderComponent, allocator);
        }

        if (prefabObj->HasComponent<SpriteRender>())
        {
            Value spriteRenderComponent(rapidjson::kObjectType);
            spriteRenderComponent.AddMember("Name", "SpriteRender", allocator);

            const auto& spriteRender = prefabObj->GetComponent<SpriteRender>();
            spriteRenderComponent.AddMember("Sprite", Value(spriteRender.texturePath.c_str(), allocator), allocator);
            spriteRenderComponent.AddMember("Shape", spriteRender.shape == SPRITE_SHAPE::BOX ? 0 : 1, allocator);

            componentsArray.PushBack(spriteRenderComponent, allocator);
        }

        if (prefabObj->HasComponent<Script>())
        {
            Value scriptComponent(rapidjson::kObjectType);
            scriptComponent.AddMember("Name", "Script", allocator);

            const auto& script = prefabObj->GetComponent<Script>();
            scriptComponent.AddMember("Path", Value(script.scriptPath.c_str(), allocator), allocator);
            scriptComponent.AddMember("ClassName", Value(script.scriptName.c_str(), allocator), allocator);

            componentsArray.PushBack(scriptComponent, allocator);
        }

	    if(prefabObj->HasComponent<Animation>())
	    {
	        Value animationComponent(rapidjson::kObjectType);
	        animationComponent.AddMember("Name", "Animation", allocator);

	    	Value clipsList(kObjectType);
	        const auto& animation = prefabObj->GetComponent<Animation>();
	        for(const auto& [key,value] : animation.clips)
	        {
	            Value clip(key.c_str(),allocator);
	        	Value clipData(kArrayType);
	        	clipData.PushBack(Value(value.keyPath.c_str(),allocator), allocator);
	        	clipData.PushBack(Value(key.c_str(),allocator), allocator);
	        	
	        	clipData.PushBack(value.pivot.x, allocator);
	        	clipData.PushBack(value.pivot.y, allocator);
	        	clipData.PushBack(value.pixelsPerUnit, allocator);
	        	clipData.PushBack(value.total_frames, allocator);
	        	clipData.PushBack(value.pixel_width, allocator);
	        	clipData.PushBack(value.pixel_height,allocator);
	        	clipData.PushBack(value.total_width, allocator);
	        	clipData.PushBack(value.total_height, allocator);
	        	clipData.PushBack(value.frame_time, allocator);
	        	clipData.PushBack(value.looping, allocator);
	        	clipsList.AddMember(clip, clipData, allocator);
	        }
			animationComponent.AddMember("Clips", clipsList, allocator);
	    	animationComponent.AddMember("CurrentClip", Value(animation.currentClip.c_str(), allocator), allocator);
	    	componentsArray.PushBack(animationComponent, allocator);
	    }
        if (prefabObj->HasComponent<Enemy>())
        {
            Value enemyComponent(rapidjson::kObjectType);

            enemyComponent.AddMember("Name", Value("EnemyComponent", allocator), allocator);

            const auto& enemy = prefabObj->GetComponent<Enemy>();

            Value position(rapidjson::kArrayType);
            position.PushBack(enemy.posX, allocator);
            position.PushBack(enemy.posY, allocator);
            enemyComponent.AddMember("Position", position, allocator);

            enemyComponent.AddMember("Type", enemy.type, allocator);

            componentsArray.PushBack(enemyComponent, allocator);
        }

        if (prefabObj->HasComponent<Player>())
        {
            Value playerComponent(rapidjson::kObjectType);
            playerComponent.AddMember("Name", "Player", allocator);

            const auto& player = prefabObj->GetComponent<Player>();
            playerComponent.AddMember("MaxHealth", player.maxHealth, allocator);
            playerComponent.AddMember("CurrentHealth", player.currentHealth, allocator);
            playerComponent.AddMember("MaxComboHits", player.maxComboHits, allocator);
            playerComponent.AddMember("CurrentComboHits", player.currentComboHits, allocator);
            playerComponent.AddMember("ComboDamage", player.comboDamage, allocator);
            playerComponent.AddMember("AttackCooldown", player.attackCooldown, allocator);
            playerComponent.AddMember("AttackTimer", player.attackTimer, allocator);
            playerComponent.AddMember("PlayerForce", player.playerForce, allocator);
            playerComponent.AddMember("IsDead", player.isDead, allocator);
            playerComponent.AddMember("CanAttack", player.canAttack, allocator);

            componentsArray.PushBack(playerComponent, allocator);
        }
        if (prefabObj->HasComponent<AudioManager>()) {
            Value audioComponent(rapidjson::kObjectType);
            audioComponent.AddMember("Name", "Audio", allocator);

            const auto& audioManager = prefabObj->GetComponent<AudioManager>();

            // Save Music List
            Value musicArray(rapidjson::kArrayType);
            for (const auto& music : audioManager.music) {
                Value musicObject(rapidjson::kObjectType);

                // Add name and path for each music entry
                musicObject.AddMember("Name", Value(music.audioName.c_str(), allocator), allocator);
                musicObject.AddMember("Path", Value(music.audioPath.c_str(), allocator), allocator);

                // Add to the music array
                musicArray.PushBack(musicObject, allocator);
            }
            audioComponent.AddMember("Music", musicArray, allocator);

            // Save SFX List
            Value sfxArray(rapidjson::kArrayType);
            for (const auto& sfx : audioManager.sfx) {
                Value sfxObject(rapidjson::kObjectType);

                // Add name and path for each SFX entry
                sfxObject.AddMember("Name", Value(sfx.audioName.c_str(), allocator), allocator);
                sfxObject.AddMember("Path", Value(sfx.audioPath.c_str(), allocator), allocator);

                // Add to the SFX array
                sfxArray.PushBack(sfxObject, allocator);
            }
            audioComponent.AddMember("SFX", sfxArray, allocator);

            // Add the audio component to the components array
            componentsArray.PushBack(audioComponent, allocator);
        }

        gameObjectData.AddMember("Components", componentsArray, allocator);


        //gameObjectsArray.PushBack(gameObjectData, allocator);
        // Add the array to the document
        document.AddMember("GameObject", gameObjectData, allocator);

        std::string file = "../Assets/" + file_name + ".json";
        if (!Serialization::PushJSON(file, document))
        {
            std::cerr << "Failed to save prefab to file: " << file << std::endl;
        }
    }

    Vec2 SceneManager::GetPlayScreen()
    {
        return GetInstance().cameraSize;
    }

    void SceneManager::SetPlayScreen(Vec2 playsize)
    {
        GetInstance().cameraSize = playsize;
    }

	void SceneManager::print_performance(std::chrono::duration<double> loop, std::chrono::duration<double> collision, std::chrono::duration<double> physics, std::chrono::duration<double> graphics)
	{
		double collision_percent = static_cast<double>((collision.count() / loop.count()) * 100.f);
		double physics_percent = static_cast<double>((physics.count() / loop.count()) * 100.f);
		double graphics_percent = static_cast<double>((graphics.count() / loop.count()) * 100.f);

		/*std::cout << "Performance View:" << std::endl;
		std::cout << "Collision: " << collision_percent << "%" << std::endl;
		std::cout << "Physics: " << physics_percent << "%" << std::endl;
		std::cout << "Graphics: " << graphics_percent << "%" << std::endl;*/

		UME_ENGINE_INFO("Performance View\n");
		UME_ENGINE_INFO("Collision: {0}%", collision_percent);
		UME_ENGINE_INFO("Physics: {0}%", physics_percent);
		UME_ENGINE_INFO("Graphics: {0}%", graphics_percent);

	}
}
