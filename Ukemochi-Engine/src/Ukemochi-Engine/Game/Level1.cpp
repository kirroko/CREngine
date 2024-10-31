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
#include "../Graphics/Renderer.h"   // for renderer system
#include "../Audio/Audio.h"			// for audio system
#include "../Graphics/Camera2D.h"

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

		// Initialize the graphics and collision system
		ECS::GetInstance().GetSystem<Renderer>()->init();
		ECS::GetInstance().GetSystem<Collision>()->Init();

		// load textures
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/Moon Floor.png"); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/Worm.png"); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/Bunny_Right_Sprite.png"); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/terrain.png"); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/running_player_sprite_sheet.png"); // load texture

		// BACKGROUND 
		GameObject level_background = GameObjectFactory::CreateObject();
		level_background.AddComponent(Transform{
				Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
				ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
				0,
				Vec2{SPRITE_SCALE * 16.f, SPRITE_SCALE * 9.f}
			});
		level_background.AddComponent(SpriteRender{ "../Assets/Textures/terrain.png" });

		//// ANIMATION OBJECT
		//GameObject animation = GameObjectFactory::CreateObject();
		//animation.AddComponent(Transform{
		//	Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
		//	ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
		//	0,
		//	Vec2{SPRITE_SCALE, SPRITE_SCALE}
		//	});
		//animation.AddComponent(SpriteRender{
		//		"../Assets/Textures/Bunny_Right_Sprite.png",
		//		SPRITE_SHAPE::BOX,
		//		0,
		//		true
		//	});

		// PLAYER OBJECT
		player_obj = GameObjectFactory::CreateObject(player_data);
		auto& p_spriteRender = player_obj.GetComponent<SpriteRender>();
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures(p_spriteRender.texturePath); // load texture
		p_spriteRender.animated = true;

		// BACKGROUND OBJECT
		GameObject background = GameObjectFactory::CreateObject();
		background.AddComponent(Transform{
				Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
				ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
				0,
				Vec2{SPRITE_SCALE * 1.5f, SPRITE_SCALE * 1.5f}
			});
		background.AddComponent(Rigidbody2D());
		background.GetComponent<Rigidbody2D>().is_kinematic = true;
		background.AddComponent(BoxCollider2D());
		background.AddComponent(SpriteRender{ "../Assets/Textures/Moon Floor.png" });

		// WORM OBJECT 1 - DYNAMIC
		worm_0 = GameObjectFactory::CreateObject();
		worm_0.AddComponent(Transform{
				Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.75f,
				ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.75f},
				0,
				Vec2{SPRITE_SCALE, SPRITE_SCALE}
			});
		worm_0.AddComponent(Rigidbody2D{ Vec2{}, Vec2{ENTITY_ACCEL, ENTITY_ACCEL}, Vec2{}, Vec2{},1.f, 1.f, 0.9f, 0.f,0.f,0.f,0.f,1.f, 1.f, 0.9f, false, false });
		worm_0.AddComponent(BoxCollider2D());
		worm_0.GetComponent<BoxCollider2D>().tag = "Enemy";
		worm_0.AddComponent(SpriteRender{
				"../Assets/Textures/Worm.png",
				SPRITE_SHAPE::BOX,
			});

		// WORM OBJECT 2 - DYNAMIC - CLONED
		GameObject worm_1 = GameObjectFactory::CloneObject(worm_0);
		worm_1.GetComponent<Transform>().position = Vec2{ ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.25f,
			ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.25f };

		// WORM OBJECT 3 - DYNAMIC - CLONED
		GameObject worm_2 = GameObjectFactory::CloneObject(worm_0);
		worm_2.GetComponent<Transform>().position = Vec2{ ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.75f,
			ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.25f };

		// Create left door entity
		GameObject door_0 = GameObjectFactory::CreateObject();
		door_0.AddComponent(Transform{
				Vec2{0,
				ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
				0,
				Vec2{SPRITE_SCALE * 0.25f, SPRITE_SCALE * 1.75f}
			});
		door_0.AddComponent(Rigidbody2D());
		door_0.GetComponent<Rigidbody2D>().is_kinematic = true;
		door_0.AddComponent(BoxCollider2D());
		door_0.GetComponent<BoxCollider2D>().is_trigger = true;
		door_0.GetComponent<BoxCollider2D>().tag = "Left Door";
		door_0.AddComponent(SpriteRender{
				"../Assets/Textures/Moon Floor.png",
				SPRITE_SHAPE::BOX
			});

		// Create right door entity
		GameObject door_1 = GameObjectFactory::CloneObject(door_0);
		door_1.GetComponent<Transform>() = Transform{
			Vec2{(float)ECS::GetInstance().GetSystem<Renderer>()->screen_width,
			ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
			0,
			Vec2{SPRITE_SCALE * 0.25f, SPRITE_SCALE * 1.75f}
		};
		door_1.GetComponent<BoxCollider2D>().tag = "Right Door";

		// Create top door entity
		GameObject door_2 = GameObjectFactory::CloneObject(door_0);
		door_2.GetComponent<Transform>() = Transform{
			Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
			0},
			0,
			Vec2{SPRITE_SCALE * 1.75f, SPRITE_SCALE * 0.25f}
		};
		door_2.GetComponent<BoxCollider2D>().tag = "Top Door";

		// Create bottom door entity
		GameObject door_3 = GameObjectFactory::CloneObject(door_0);
		door_3.GetComponent<Transform>() = Transform{
			Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
			(float)ECS::GetInstance().GetSystem<Renderer>()->screen_height},
			0,
			Vec2{SPRITE_SCALE * 1.75f, SPRITE_SCALE * 0.25f}
		};
		door_3.GetComponent<BoxCollider2D>().tag = "Btm Door";

		// ANIMATION OBJECT
		/*GameObject animation = GameObjectFactory::CreateObject();
		animation.AddComponent(Transform{
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
			Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.8f,
			ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
			0,
			Vec2{SPRITE_SCALE * 0.5f, SPRITE_SCALE * 0.5f}
			});
		circle.AddComponent(SpriteRender{ "", SPRITE_SHAPE::CIRCLE });*/

		// Set the player object in the Renderer
		ECS::GetInstance().GetSystem<Renderer>()->SetPlayerObject(player_obj);
		
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
			ECS::GetInstance().GetSystem<Physics>()->AddForceX(player_rb, -PLAYER_FORCE);
		// Press 'D' or right key to move the player to the right
		else if (Input::IsKeyPressed(UME_KEY_D))
			ECS::GetInstance().GetSystem<Physics>()->AddForceX(player_rb, PLAYER_FORCE);
		else
			ECS::GetInstance().GetSystem<Physics>()->RemoveForceX(player_rb); // Stop moving the player in the x axis

		// Player Input for rotation, to test rotate physics
		if (Input::IsKeyPressed(UME_KEY_R))
			ECS::GetInstance().GetSystem<Physics>()->AddTorque(player_rb, PLAYER_FORCE);
		else
			ECS::GetInstance().GetSystem<Physics>()->RemoveTorque(player_rb);

		// Renderer Inputs
		if (Ukemochi::Input::IsKeyTriggered(GLFW_KEY_T))
			ECS::GetInstance().GetSystem<Renderer>()->ToggleInputsForScale();
		else if (Ukemochi::Input::IsKeyTriggered(GLFW_KEY_Y))
			ECS::GetInstance().GetSystem<Renderer>()->ToggleInputsForRotation();
		else if (Ukemochi::Input::IsKeyTriggered(GLFW_KEY_U))
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
			GameObject clone = GameObjectFactory::CloneObject(worm_0);
			clone.GetComponent<Transform>().position = Vec2{ clone.GetComponent<Transform>().position.x + 5.f, clone.GetComponent<Transform>().position.y + 1.f };
		}

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
	}

	void Level1_Draw()//rendering of the game for Level1
	{
		// Render the entities
		ECS::GetInstance().GetSystem<Renderer>()->renderToFramebuffer();
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