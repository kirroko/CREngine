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

// System Includes
#include "../Input/Input.h"			// for input system
#include "../Physics/Physics.h"	    // for physics system
#include "../Collision/Collision.h" // for collision system
#include "../Graphics/Renderer.h"   // for renderer system
#include "../Audio/Audio.h"			//for audio

namespace Ukemochi
{
	// --- TEMP player variables ---
	EntityID player_entity;
	const float SPRITE_SCALE = 100.f;
	const float ENTITY_ACCEL = 250.f;
	float audioVolume = 0.04f;
	std::string player_data{ "../Assets/Player.json" };
	GameObject player_obj;
	GameObject worm_0;

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

		// Initialize the graphics system
		ECS::GetInstance().GetSystem<Renderer>()->init();
		// load textures
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/Moon Floor.png"); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/Worm.png"); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/Bunny_Right_Sprite.png"); // load texture
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures("../Assets/Textures/terrain.png"); // load texture

		// BACKGROUND 
		GameObject level_background = GameObjectFactory::CreateObject();
		level_background.AddComponent(Transform{
				Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
				ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
				0,
				Vec2{SPRITE_SCALE * 16.f, SPRITE_SCALE * 9.f}
			});
		level_background.AddComponent(SpriteRender{ "../Assets/Textures/terrain.png" });

		// PLAYER OBJECT
		player_obj = GameObjectFactory::CreateObject(player_data);
		auto& p_spriteRender = player_obj.GetComponent<SpriteRender>();
		ECS::GetInstance().GetSystem<Renderer>()->setUpTextures(p_spriteRender.texturePath); // load texture

		// BACKGROUND OBJECT
		GameObject background = GameObjectFactory::CreateObject();
		background.AddComponent(Transform{
				Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
				ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
				0,
				Vec2{SPRITE_SCALE * 1.5f, SPRITE_SCALE * 1.5f}
			});
		background.AddComponent(Rigidbody2D{ Vec2{}, Vec2{}, true });
		background.AddComponent(BoxCollider2D{ Vec2{}, Vec2{} });
		background.AddComponent(SpriteRender{ "../Assets/Textures/Moon Floor.png" });

		// WORM OBJECT 1 - DYNAMIC
		worm_0 = GameObjectFactory::CreateObject();
		worm_0.AddComponent(Transform{
				Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.75f,
				ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.75f},
				0,
				Vec2{SPRITE_SCALE, SPRITE_SCALE}
			});
		worm_0.AddComponent(Rigidbody2D{ Vec2{ENTITY_ACCEL, ENTITY_ACCEL}, Vec2{ENTITY_ACCEL, ENTITY_ACCEL} });
		worm_0.AddComponent(BoxCollider2D{ Vec2{}, Vec2{} });
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
		door_0.AddComponent(Rigidbody2D{ Vec2{}, Vec2{}, true });
		door_0.AddComponent(BoxCollider2D{ Vec2{}, Vec2{}, 0, false, true });
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

		// Create top door entity
		GameObject door_2 = GameObjectFactory::CloneObject(door_0);
		door_2.GetComponent<Transform>() = Transform{
			Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
			0},
			0,
			Vec2{SPRITE_SCALE * 1.75f, SPRITE_SCALE * 0.25f}
		};

		// Create bottom door entity
		GameObject door_3 = GameObjectFactory::CloneObject(door_0);
		door_3.GetComponent<Transform>() = Transform{
			Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
			(float)ECS::GetInstance().GetSystem<Renderer>()->screen_height},
			0,
			Vec2{SPRITE_SCALE * 1.75f, SPRITE_SCALE * 0.25f}
		};

		// ANIMATION OBJECT
		GameObject animation = GameObjectFactory::CreateObject();
		animation.AddComponent(Transform{
			Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
			ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
			0,
			Vec2{SPRITE_SCALE, SPRITE_SCALE}
			});
		animation.AddComponent(SpriteRender{
				"../Assets/Textures/Bunny_Right_Sprite.png",
				SPRITE_SHAPE::BOX,
				0,
				true
			});

		
	}

	void Level1_Update()//Level1 game runtime
	{
		// --- Handle User Input for Player controls ---
		// Press 'W' or up key to move the player up
		auto& player_rb = player_obj.GetComponent<Rigidbody2D>();
		if (UME::Input::IsKeyPressed(UME_KEY_W) || UME::Input::IsKeyPressed(UME_KEY_UP))
			player_rb.velocity.y = -player_rb.acceleration.y;
		// Press 'S' or down key to move the player down
		else if (UME::Input::IsKeyPressed(UME_KEY_S) || UME::Input::IsKeyPressed(UME_KEY_DOWN))
			player_rb.velocity.y = player_rb.acceleration.y;
		else
			player_rb.velocity.y = 0.0f; // Stop moving the player in the y axis

		// Press 'A' or left key to move the player left
		if (UME::Input::IsKeyPressed(UME_KEY_A) || UME::Input::IsKeyPressed(UME_KEY_LEFT))
			player_rb.velocity.x = -player_rb.acceleration.x;
		// Press 'D' or right key to move the player to the right
		else if (UME::Input::IsKeyPressed(UME_KEY_D) || UME::Input::IsKeyPressed(UME_KEY_RIGHT))
			player_rb.velocity.x = player_rb.acceleration.x;
		else
			player_rb.velocity.x = 0.0f; // Stop moving the player in the x axis

		if (UME::Input::IsKeyTriggered(GLFW_KEY_T))
			ECS::GetInstance().GetSystem<Renderer>()->ToggleInputsForScale();
		else if (UME::Input::IsKeyTriggered(GLFW_KEY_Y))
			ECS::GetInstance().GetSystem<Renderer>()->ToggleInputsForRotation();
		else if (UME::Input::IsKeyTriggered(GLFW_KEY_U))
			ECS::GetInstance().GetSystem<Renderer>()->debug_mode_enabled = static_cast<GLboolean>(!ECS::GetInstance().GetSystem<Renderer>()->debug_mode_enabled);
		// --- End User Input ---

		// Update the entities physics
		ECS::GetInstance().GetSystem<Physics>()->UpdatePhysics();

		// Check collisions between the entities
		ECS::GetInstance().GetSystem<Collision>()->CheckCollisions();

		if (UME::Input::IsKeyTriggered(GLFW_KEY_P))
		{
			audioVolume -= 0.02f;
			audioVolume = audioVolume < 0.f ? 0.f : audioVolume;
			Audio::GetInstance().SetAudioVolume(BGM, audioVolume);
		}
		if (UME::Input::IsKeyTriggered(GLFW_KEY_O))
		{
			audioVolume += 0.02f;
			audioVolume = audioVolume > 1.f ? 1.f : audioVolume;
			Audio::GetInstance().SetAudioVolume(BGM, audioVolume);
		}
		if (UME::Input::IsKeyPressed(GLFW_KEY_M))
		{
			Audio::GetInstance().StopAllSoundsInGroup(LEVEL1);
		}
		if (UME::Input::IsKeyPressed(GLFW_KEY_N))
		{
			Audio::GetInstance().PlayAllSoundsInGroup(LEVEL1);
		}

		//test run_time cloning
		if (UME::Input::IsKeyTriggered(GLFW_KEY_L))
		{
			std::cout << "Cloning Mob Object\n";
			GameObject clone = GameObjectFactory::CloneObject(worm_0);
			clone.GetComponent<Transform>().position = Vec2{ clone.GetComponent<Transform>().position.x + 5.f, clone.GetComponent<Transform>().position.y + 1.f };
		}
	}

	void Level1_Draw()//rendering of the game for Level1
	{
		// Render the entities
		ECS::GetInstance().GetSystem<Renderer>()->render();
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