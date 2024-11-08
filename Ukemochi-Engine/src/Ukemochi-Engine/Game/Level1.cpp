/* Start Header ************************************************************************/
/*!
\file    Level1.cpp
\author		Tan Si Han, t.sihan, 2301264. t.sihan\@digipen.edu
\co-authors Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\co-authors Pek Jun Kai Gerald, p.junkaigerald, 2301334, p.junkaigerald\@digipen.edu (5%)
\date    20/09/2024
\brief   This files contains the definition of functions in Level1.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#include "PreCompile.h"

#include "GSM.h"
#include "Level1.h"
#include <iostream> // for std::cout

#include "../Factory/Factory.h"		//for Game Objects and its related functions
#include "../ECS/ECS.h"				// for ECS
#include "../ECS/Components.h"		// for Rigidbody2D, BoxCollider2D component

#include "../Graphics/textRenderer.h"
// System Includes
#include "../Input/Input.h"			// for input system
#include "../Physics/Physics.h"	    // for physics system
#include "../Collision/Collision.h" // for collision system
#include "../Math/Transformation.h" // for transformation system
#include "../Graphics/Renderer.h"   // for renderer system
#include "../Audio/Audio.h"			// for audio system
#include "../Graphics/Camera2D.h"
#include "../InGameGUI/InGameGUI.h" // for in game GUI system
#include "Ukemochi-Engine/Factory/GameObjectManager.h"

namespace Ukemochi
{
	// --- TEMP player variables ---
	const float SPRITE_SCALE = 100.f;
	const float ENTITY_ACCEL = 750.f;
	const float PLAYER_FORCE = 750.f;
	float audioVolume = 0.04f;
	std::string player_data{ "../Assets/Player.json" };
	GameObject player_obj;
	GameObject worm_0;
	Renderer time;
	
	GLfloat lastFrameTime = 0.0f;
	GLfloat deltaTime = 0.0f;

	void Level1_Load()//Load all necessary assets before start of Level1
	{
		//std::cout << "Level1:Load" << '\n';
		Audio::GetInstance().LoadSound(R"(../Assets/Audio/BGM_game.mp3)");
		Audio::GetInstance().LoadSound(R"(../Assets/Audio/SFX_jump.wav)");
	}

	void Level1_Initialize()//Initialize the game at the start of Level1
	{
		Audio::GetInstance().PlaySoundInGroup(AudioList::BGM, ChannelGroups::LEVEL1);
		Audio::GetInstance().SetAudioVolume(BGM, audioVolume);
		//std::cout << "Level1:Initialize" << '\n';
		ECS::GetInstance().GetSystem<Renderer>()->setUpShaders();

		// load textures
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/terrain.png", ECS::GetInstance().GetSystem<Renderer>()->current_texture_index); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/Moon Floor.png", ECS::GetInstance().GetSystem<Renderer>()->current_texture_index); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/Worm.png", ECS::GetInstance().GetSystem<Renderer>()->current_texture_index); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/Bunny_Right_Sprite.png", ECS::GetInstance().GetSystem<Renderer>()->current_texture_index); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/running_player_sprite_sheet.png", ECS::GetInstance().GetSystem<Renderer>()->current_texture_index); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/UI/pause.png", ECS::GetInstance().GetSystem<Renderer>()->current_texture_index); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/UI/base.png", ECS::GetInstance().GetSystem<Renderer>()->current_texture_index); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/UI/game_logo.png", ECS::GetInstance().GetSystem<Renderer>()->current_texture_index); // load texture

		// Initialize the renderer and collision systems
		ECS::GetInstance().GetSystem<Renderer>()->init();
		ECS::GetInstance().GetSystem<Collision>()->Init();

		// BACKGROUND
		// GameObject level_background = GameObjectFactory::CreateObject();
		GameObject level_background = GameObjectManager::GetInstance().CreateObject("level_background");
		level_background.AddComponent(Transform{
			Mtx44{},
			Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
			ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
			0,
			Vec2{SPRITE_SCALE * 16.f, SPRITE_SCALE * 9.f}
		});
		level_background.AddComponent(SpriteRender{ "../Assets/Textures/terrain.png" });

		// PLAYER OBJECT
		// player_obj = GameObjectFactory::CreateObject(player_data);
		player_obj = GameObjectManager::GetInstance().CreatePrefabObject(player_data);
		auto& p_spriteRender = player_obj.GetComponent<SpriteRender>();
		p_spriteRender.textureID = ECS::GetInstance().GetSystem<Renderer>()->current_texture_index;
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures(p_spriteRender.texturePath, ECS::GetInstance().GetSystem<Renderer>()->current_texture_index); // load texture
		std::cout << ECS::GetInstance().GetSystem<Renderer>()->current_texture_index << std::endl;
		p_spriteRender.animated = true;

		// BACKGROUND OBJECT
		// GameObject background = GameObjectFactory::CreateObject()
		GameObject background = GameObjectManager::GetInstance().CreateObject("Background");
		background.AddComponent(Transform{
			Mtx44{},
			Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
			ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
			0,
			Vec2{SPRITE_SCALE * 1.5f, SPRITE_SCALE * 1.5f}
		});
		background.AddComponent(Rigidbody2D());
		background.GetComponent<Rigidbody2D>().is_kinematic = true;
		background.AddComponent(BoxCollider2D());
		background.AddComponent(SpriteRender{ "../Assets/Textures/Moon Floor.png" });

		// // WORM OBJECT 1 - DYNAMIC
		// // worm_0 = GameObjectFactory::CreateObject();
		// worm_0 = GameObjectManager::GetInstance().CreateObject("Worm_0","Enemy");
		// worm_0.AddComponent(Transform{
		// 	Mtx44{},
		// 	Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.75f,
		// 	ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.75f},
		// 	0,
		// 	Vec2{SPRITE_SCALE, SPRITE_SCALE}
		// });
		// worm_0.AddComponent(Rigidbody2D{ Vec2{}, Vec2{ENTITY_ACCEL, ENTITY_ACCEL}, Vec2{}, Vec2{},1.f, 1.f, 0.9f, 0.f,0.f,0.f,0.f,1.f, 1.f, 0.9f, false, false });
		// worm_0.AddComponent(BoxCollider2D());
		// worm_0.GetComponent<BoxCollider2D>().tag = "Enemy";
		// worm_0.AddComponent(SpriteRender{
		// 		"../Assets/Textures/Worm.png",
		// 		SPRITE_SHAPE::BOX,
		// 	});
		//
		// // WORM OBJECT 2 - DYNAMIC - CLONED
		// // GameObject worm_1 = GameObjectFactory::CloneObject(worm_0);
		// GameObject worm_1 = GameObjectManager::GetInstance().CloneObject(worm_0,"Worm_1","Enemy");
		// worm_1.GetComponent<Transform>().position = Vec2{ ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.25f,
		// 	ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.25f };
		//
		// // WORM OBJECT 3 - DYNAMIC - CLONED
		// // GameObject worm_2 = GameObjectFactory::CloneObject(worm_0, TODO, TODO);
		// GameObject worm_2 = GameObjectManager::GetInstance().CloneObject(worm_0, "Worm_2", "Enemy");
		// worm_2.GetComponent<Transform>().position = Vec2{ ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.75f,
		// 	ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.25f };
		//
		// // Create left door entity
		// // GameObject door_0 = GameObjectFactory::CreateObject();
		// GameObject door_0 = GameObjectManager::GetInstance().CreateObject("Door_0");
		// door_0.AddComponent(Transform{
		// 	Mtx44{},
		// 	Vec2{0,
		// 	ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
		// 	0,
		// 	Vec2{SPRITE_SCALE * 0.25f, SPRITE_SCALE * 1.75f}
		// });
		// door_0.AddComponent(Rigidbody2D());
		// door_0.GetComponent<Rigidbody2D>().is_kinematic = true;
		// door_0.AddComponent(BoxCollider2D());
		// door_0.GetComponent<BoxCollider2D>().is_trigger = true;
		// door_0.GetComponent<BoxCollider2D>().tag = "Left Door";
		// door_0.AddComponent(SpriteRender{
		// 		"../Assets/Textures/Moon Floor.png",
		// 		SPRITE_SHAPE::BOX
		// 	});
		//
		// // Create right door entity
		// // GameObject door_1 = GameObjectFactory::CloneObject(door_0, TODO, TODO);
		// GameObject door_1 = GameObjectManager::GetInstance().CloneObject(door_0, "Door_1");
		// door_1.GetComponent<Transform>() = Transform{
		// 	Mtx44{},
		// 	Vec2{(float)ECS::GetInstance().GetSystem<Renderer>()->screen_width,
		// 	ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
		// 	0,
		// 	Vec2{SPRITE_SCALE * 0.25f, SPRITE_SCALE * 1.75f}
		// };
		// door_1.GetComponent<BoxCollider2D>().tag = "Right Door";
		//
		// // Create top door entity
		// // GameObject door_2 = GameObjectFactory::CloneObject(door_0, TODO, TODO);
		// GameObject door_2 = GameObjectManager::GetInstance().CloneObject(door_0, "Door_2");
		// door_2.GetComponent<Transform>() = Transform{
		// 	Mtx44{},
		// 	Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
		// 	0},
		// 	0,
		// 	Vec2{SPRITE_SCALE * 1.75f, SPRITE_SCALE * 0.25f}
		// };
		// door_2.GetComponent<BoxCollider2D>().tag = "Top Door";
		//
		// // Create bottom door entity
		// // GameObject door_3 = GameObjectFactory::CloneObject(door_0, TODO, TODO);
		// GameObject door_3 = GameObjectManager::GetInstance().CloneObject(door_0, "Door_3");
		// door_3.GetComponent<Transform>() = Transform{
		// 	Mtx44{},
		// 	Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
		// 	(float)ECS::GetInstance().GetSystem<Renderer>()->screen_height},
		// 	0,
		// 	Vec2{SPRITE_SCALE * 1.75f, SPRITE_SCALE * 0.25f}
		// };
		// door_3.GetComponent<BoxCollider2D>().tag = "Btm Door";

		// ANIMATION OBJECT
		/*GameObject animation = GameObjectFactory::CreateObject();
		animation.AddComponent(Transform{
			Mtx44{},
			Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
			ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
			0,
			Vec2{SPRITE_SCALE, SPRITE_SCALE}
			});
		animation.AddComponent(SpriteRender{
				"../Assets/Textures/running_player_sprite_sheet.png",
				SPRITE_SHAPE::BOX,
				0,
				true
			});*/

		//// BACKGROUND 
		//GameObject level_background = GameObjectFactory::CreateObject();
		//level_background.AddComponent(Transform{
		//		Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
		//		ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
		//		0,
		//		Vec2{SPRITE_SCALE * 16.f, SPRITE_SCALE * 9.f}
		//	});
		//level_background.AddComponent(SpriteRender{ "../Assets/Textures/terrain.png" });
		
		// Circle Creation for Testing
		/*GameObject circle = GameObjectFactory::CreateObject();
		circle.AddComponent(Transform{
			Mtx44{},
			Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.2f,
			ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
			0,
			Vec2{SPRITE_SCALE * 0.5f, SPRITE_SCALE * 0.5f}
			});
		circle.AddComponent(SpriteRender{ "../Assets/Textures/terrain.png", SPRITE_SHAPE::CIRCLE });*/

		// Set the player object in the Renderer
		ECS::GetInstance().GetSystem<Renderer>()->SetPlayerObject(player_obj);

		// Set the player object in the Transformation
		ECS::GetInstance().GetSystem<Transformation>()->playerObject = &player_obj;
		
		// Initialize the in game GUI system
		ECS::GetInstance().GetSystem<InGameGUI>()->Init();
	}

	void Level1_Update()//Level1 game runtime
	{
		// --- HANDLE USER INPUTS ---

		// Player Inputs for movement
		auto& player_rb = player_obj.GetComponent<Rigidbody2D>();
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
		auto& player_trans = player_obj.GetComponent<Transform>();
		if (Input::IsKeyPressed(UME_KEY_F))
			ECS::GetInstance().GetSystem<Transformation>()->IncreaseScale(player_trans);
		else if (Input::IsKeyPressed(UME_KEY_G))
			ECS::GetInstance().GetSystem<Transformation>()->DecreaseScale(player_trans);

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
		if (Ukemochi::Input::IsKeyTriggered(GLFW_KEY_P))
		{
			audioVolume -= 0.02f;
			audioVolume = audioVolume < 0.f ? 0.f : audioVolume;
			Audio::GetInstance().SetAudioVolume(BGM, audioVolume);
		}
		if (Ukemochi::Input::IsKeyTriggered(GLFW_KEY_O))
		{
			audioVolume += 0.02f;
			audioVolume = audioVolume > 1.f ? 1.f : audioVolume;
			Audio::GetInstance().SetAudioVolume(BGM, audioVolume);
		}
		if (Ukemochi::Input::IsKeyPressed(GLFW_KEY_M))
		{
			Audio::GetInstance().StopAllSoundsInGroup(LEVEL1);
		}
		if (Ukemochi::Input::IsKeyPressed(GLFW_KEY_N))
		{
			Audio::GetInstance().PlayAllSoundsInGroup(LEVEL1);
		}

		// Game Object Inputs
		if (Ukemochi::Input::IsKeyTriggered(GLFW_KEY_L)) //test run_time cloning
		{
			std::cout << "Cloning Mob Object\n";
			GameObject clone = GameObjectManager::GetInstance().CloneObject(worm_0, "Worm", "Enemy");
			clone.GetComponent<Transform>().position = Vec2{ clone.GetComponent<Transform>().position.x + 5.f, clone.GetComponent<Transform>().position.y + 1.f };
		}

		// Update the in game GUI inputs
		ECS::GetInstance().GetSystem<InGameGUI>()->Update();

		// Camera
		GLfloat currentFrameTime = static_cast<GLfloat>(glfwGetTime());
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;
		ECS::GetInstance().GetSystem<Camera>()->processCameraInput(deltaTime);

		// Animation
		// Check for slow motion toggle key (M key in this case)
		if (Ukemochi::Input::IsKeyTriggered(GLFW_KEY_Z))
		{
			ECS::GetInstance().GetSystem<Renderer>()->toggleSlowMotion();
		}
		
		ECS::GetInstance().GetSystem<Renderer>()->animationKeyInput();
		
		// --- END USER INPUTS ---

		// --- GAME LOGIC UPDATE ---


		// --- PHYSICS UPDATE ---
		// Update the entities physics
		ECS::GetInstance().GetSystem<Physics>()->UpdatePhysics();

		// --- COLLISION UPDATE ---
		// Check the collisions between the entities
		ECS::GetInstance().GetSystem<Collision>()->CheckCollisions();

		// --- TRANSFORMATION UPDATE ---
		// Compute the entities transformations
		ECS::GetInstance().GetSystem<Transformation>()->ComputeTransformations();
	}

	void Level1_Draw()//rendering of the game for Level1
	{
		// Render the entities
		ECS::GetInstance().GetSystem<Renderer>()->renderToFramebuffer();
		//ECS::GetInstance().GetSystem<Renderer>()->render();

	}

	void Level1_Free()//release unused assets/variable memories
	{
	}

	void Level1_Unload()//unload the level
	{
		// Unload the renderer system
		ECS::GetInstance().GetSystem<Renderer>()->cleanUp();
		Audio::GetInstance().StopAllSoundsInGroup(ChannelGroups::LEVEL1);
	}
}
