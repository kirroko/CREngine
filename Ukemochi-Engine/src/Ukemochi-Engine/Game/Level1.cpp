/* Start Header ************************************************************************/
/*!
\file    Level1.cpp
\author  Tan Si Han, t.sihan, 2301264
\co-authors Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\par     t.sihan@digipen.edu
\date    20/09/2024
\brief   This files contains the definction of functions in Level1.


Copyright (C) 2024 DigiPen Institute of Technology.  Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#include "PreCompile.h"

#include "GSM.h"
#include "Level1.h"
#include <iostream> // for std::cout

#include "../ECS/ECS.h"		   // for ECS
#include "../ECS/Components.h" // for Rigidbody2D, BoxCollider2D component

// System Includes
#include "../Input/Input.h"			// for input system
#include "../Physics/Physics.h"	    // for physics system
#include "../Collision/Collision.h" // for collision system
#include "../Graphics/Renderer.h"   // for renderer system
#include "../Audio/Audio.h"			//for audio
#include "../Factory/Factory.h"		//for Game Objects and its related functions

namespace Ukemochi
{
	// --- TEMP player variables ---
	EntityID player_entity;
	const float SPRITE_SCALE = 100.f;
	const float ENTITY_ACCEL = 250.f;
	float audioVolume = 1;
	std::string player_data{ "../Assets/Player.json" };
	GameObject player_obj;

	void Level1_Load()//Load all necessary assets before start of Level1
	{
		//some code here
		//std::cout << "Level1:Load" << '\n';
		//audio.CreateGroup("test");
		//audio.LoadSound(R"(C:\Users\tansi\OneDrive\Desktop\BGM_game.mp3)");

		//Audio::GetInstance().LoadSound(R"(C:\Users\tansi\OneDrive\Desktop\BGM_game.mp3)"); //TODO: Make in relation to project
		//Audio::GetInstance().LoadSound(R"(C:\Users\tansi\OneDrive\Desktop\SFX_jump.wav)");
	}

	void Level1_Initialize()//Initialize the game at the start of Level1
	{
		Audio::GetInstance().PlaySoundInGroup(AudioList::BGM, ChannelGroups::LEVEL1);
		//some code here
		//std::cout << "Level1:Initialize" << '\n';

		// Initialize the graphics system
		ECS::GetInstance().GetSystem<Renderer>()->init();

		// Create background entity
		/*EntityID entity = ECS::GetInstance().CreateEntity();
		ECS::GetInstance().AddComponent(entity, Transform{
				Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
				ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
				Vec2{},
				Vec2{1600, 900}
			});
		ECS::GetInstance().AddComponent(entity, SpriteRender{
				"../Assets/Textures/Background.png",
				SPRITE_SHAPE::BOX,
				true,
				1.0f
			});*/

		// Create player entity
		/*player_entity = ECS::GetInstance().CreateEntity();
		ECS::GetInstance().AddComponent(player_entity, Transform{
				Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
				ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.75f},
				Vec2{},
				Vec2{SPRITE_SCALE, SPRITE_SCALE}
			});
		ECS::GetInstance().AddComponent(player_entity, Rigidbody2D{ Vec2{}, Vec2{ENTITY_ACCEL, ENTITY_ACCEL}, false });
		ECS::GetInstance().AddComponent(player_entity, BoxCollider2D{ Vec2{}, Vec2{}, 0, true });
		ECS::GetInstance().AddComponent(player_entity, SpriteRender{
				"../Assets/Textures/Ukemochi.png",
				SPRITE_SHAPE::BOX,
				true,
				1.0f
			});*/
		player_obj = GameObjectFactory::CreateObject(player_data);

		// Create static entity
		EntityID entity = ECS::GetInstance().CreateEntity();
		ECS::GetInstance().AddComponent(entity, Transform{
				Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
				ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
				Vec2{},
				Vec2{SPRITE_SCALE * 1.5f, SPRITE_SCALE * 1.5f}
			});
		ECS::GetInstance().AddComponent(entity, Rigidbody2D{ Vec2{}, Vec2{}, true });
		ECS::GetInstance().AddComponent(entity, BoxCollider2D{ Vec2{}, Vec2{} });
		ECS::GetInstance().AddComponent(entity, SpriteRender{
				"../Assets/Textures/Moon Floor.png",
				SPRITE_SHAPE::BOX,
				true,
				1.0f
			});

		// Create dynamic entity
		entity = ECS::GetInstance().CreateEntity();
		ECS::GetInstance().AddComponent(entity, Transform{
				Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.75f,
				ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.75f},
				Vec2{},
				Vec2{SPRITE_SCALE, SPRITE_SCALE}
			});
		ECS::GetInstance().AddComponent(entity, Rigidbody2D{ Vec2{ENTITY_ACCEL, ENTITY_ACCEL}, Vec2{ENTITY_ACCEL, ENTITY_ACCEL} });
		ECS::GetInstance().AddComponent(entity, BoxCollider2D{ Vec2{}, Vec2{} });
		ECS::GetInstance().AddComponent(entity, SpriteRender{
				"../Assets/Textures/Worm.png",
				SPRITE_SHAPE::BOX,
				true,
				1.0f
			});

		// Create dynamic entity
		entity = ECS::GetInstance().CreateEntity();
		ECS::GetInstance().AddComponent(entity, Transform{
				Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.25f,
				ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.25f},
				Vec2{},
				Vec2{SPRITE_SCALE, SPRITE_SCALE}
			});
		ECS::GetInstance().AddComponent(entity, Rigidbody2D{ Vec2{ENTITY_ACCEL, ENTITY_ACCEL}, Vec2{ENTITY_ACCEL, ENTITY_ACCEL} });
		ECS::GetInstance().AddComponent(entity, BoxCollider2D{ Vec2{}, Vec2{} });
		ECS::GetInstance().AddComponent(entity, SpriteRender{
				"../Assets/Textures/Worm.png",
				SPRITE_SHAPE::BOX,
				true,
				1.0f
			});

		// Create left door entity
		entity = ECS::GetInstance().CreateEntity();
		ECS::GetInstance().AddComponent(entity, Transform{
				Vec2{0,
				ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
				Vec2{},
				Vec2{SPRITE_SCALE * 0.25f, SPRITE_SCALE * 1.75f}
			});
		ECS::GetInstance().AddComponent(entity, Rigidbody2D{ Vec2{}, Vec2{}, true });
		ECS::GetInstance().AddComponent(entity, BoxCollider2D{ Vec2{}, Vec2{}, 0, false, true });
		ECS::GetInstance().AddComponent(entity, SpriteRender{
				"../Assets/Textures/Moon Floor.png",
				SPRITE_SHAPE::BOX,
				true,
				1.0f
			});

		// Create right door entity
		entity = ECS::GetInstance().CreateEntity();
		ECS::GetInstance().AddComponent(entity, Transform{
				Vec2{(float)ECS::GetInstance().GetSystem<Renderer>()->screen_width,
				ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
				Vec2{},
				Vec2{SPRITE_SCALE * 0.25f, SPRITE_SCALE * 1.75f}
			});
		ECS::GetInstance().AddComponent(entity, Rigidbody2D{ Vec2{}, Vec2{}, true });
		ECS::GetInstance().AddComponent(entity, BoxCollider2D{ Vec2{}, Vec2{}, 0, false, true });
		ECS::GetInstance().AddComponent(entity, SpriteRender{
				"../Assets/Textures/Moon Floor.png",
				SPRITE_SHAPE::BOX,
				true,
				1.0f
			});

		// Create top door entity
		entity = ECS::GetInstance().CreateEntity();
		ECS::GetInstance().AddComponent(entity, Transform{
				Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
				0},
				Vec2{},
				Vec2{SPRITE_SCALE * 1.75f, SPRITE_SCALE * 0.25f}
			});
		ECS::GetInstance().AddComponent(entity, Rigidbody2D{ Vec2{}, Vec2{}, true });
		ECS::GetInstance().AddComponent(entity, BoxCollider2D{ Vec2{}, Vec2{}, 0, false, true });
		ECS::GetInstance().AddComponent(entity, SpriteRender{
				"../Assets/Textures/Moon Floor.png",
				SPRITE_SHAPE::BOX,
				true,
				1.0f
			});

		// Create bottom door entity
		entity = ECS::GetInstance().CreateEntity();
		ECS::GetInstance().AddComponent(entity, Transform{
				Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
				(float)ECS::GetInstance().GetSystem<Renderer>()->screen_height},
				Vec2{},
				Vec2{SPRITE_SCALE * 1.75f, SPRITE_SCALE * 0.25f}
			});
		ECS::GetInstance().AddComponent(entity, Rigidbody2D{ Vec2{}, Vec2{}, true });
		ECS::GetInstance().AddComponent(entity, BoxCollider2D{ Vec2{}, Vec2{}, 0, false, true });
		ECS::GetInstance().AddComponent(entity, SpriteRender{
				"../Assets/Textures/Moon Floor.png",
				SPRITE_SHAPE::BOX,
				true,
				1.0f
			});

		//std::default_random_engine generator;
		//std::uniform_real_distribution<float> randPosition(450.0f, 800.0f);
		//std::uniform_real_distribution<float> randRotation(0.0f, 3.0f);
		//std::uniform_real_distribution<float> randScale(100.0f, 150.f);

		//float scale = randScale(generator);

		//std::vector<EntityID> entities(MAX_ENTITIES-9);
		//// Create entities
		//for (auto& entity : entities)
		//{
		//	entity = ECS::GetInstance().CreateEntity();
		//	ECS::GetInstance().AddComponent(entity, Transform{
		//		Vec2(randPosition(generator),randPosition(generator)),
		//		Vec2(randRotation(generator),randRotation(generator)),
		//		Vec2(scale,scale)
		//		});
		//	//ECS::GetInstance().AddComponent(entity, Rigidbody2D());
		//	ECS::GetInstance().AddComponent(entity, SpriteRender{
		//		"../Assets/Textures/Worm.png",
		//		SPRITE_SHAPE::BOX,
		//		true,
		//		1.0f
		//		});
		//}

		//audio.PlaySoundInGroup(AudioList::BGM, ChannelGroups::MENUAUDIO);
	}

	void Level1_Update()//Level1 game runtime
	{

		//some code here
		//std::cout << "Level1:Update" << '\n';

		// --- Handle User Input for Player controls ---
		// Press 'W' or up key to move the player up
		if (UME::Input::IsKeyPressed(UME_KEY_W) || UME::Input::IsKeyPressed(UME_KEY_UP))
		{
			//auto& player_rb = ECS::GetInstance().GetComponent<Rigidbody2D>(player_entity);
			auto& player_rb = player_obj.GetComponent<Rigidbody2D>();
			player_rb.velocity.y = -player_rb.acceleration.y;
		}
		// Press 'S' or down key to move the player down
		else if (UME::Input::IsKeyPressed(UME_KEY_S) || UME::Input::IsKeyPressed(UME_KEY_DOWN))
		{
			//auto& player_rb = ECS::GetInstance().GetComponent<Rigidbody2D>(player_entity);
			auto& player_rb = player_obj.GetComponent<Rigidbody2D>();
			player_rb.velocity.y = player_rb.acceleration.y;
		}
		else
		{
			//auto& player_rb = ECS::GetInstance().GetComponent<Rigidbody2D>(player_entity);
			auto& player_rb = player_obj.GetComponent<Rigidbody2D>();
			player_rb.velocity.y = 0.0f; // Stop moving the player in the y axis
		}

		// Press 'A' or left key to move the player left
		if (UME::Input::IsKeyPressed(UME_KEY_A) || UME::Input::IsKeyPressed(UME_KEY_LEFT))
		{
			//auto& player_rb = ECS::GetInstance().GetComponent<Rigidbody2D>(player_entity);
			auto& player_rb = player_obj.GetComponent<Rigidbody2D>();
			player_rb.velocity.x = -player_rb.acceleration.x;
		}
		// Press 'D' or right key to move the player to the right
		else if (UME::Input::IsKeyPressed(UME_KEY_D) || UME::Input::IsKeyPressed(UME_KEY_RIGHT))
		{
			//auto& player_rb = ECS::GetInstance().GetComponent<Rigidbody2D>(player_entity);
			auto& player_rb = player_obj.GetComponent<Rigidbody2D>();
			player_rb.velocity.x = player_rb.acceleration.x;
		}
		else
		{
			//auto& player_rb = ECS::GetInstance().GetComponent<Rigidbody2D>(player_entity);
			auto& player_rb = player_obj.GetComponent<Rigidbody2D>();
			player_rb.velocity.x = 0.0f; // Stop moving the player in the x axis
		}

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

		if (UME::Input::IsKeyPressed(GLFW_KEY_P))
		{
			audioVolume -= 0.01f;
			audioVolume = audioVolume < 0 ? 0 : audioVolume;
			//Audio::GetInstance().SetGroupVolume(ChannelGroups::MENUAUDIO, audioVolume);
			Audio::GetInstance().SetAudioVolume(BGM, audioVolume);
		}
		if (UME::Input::IsKeyPressed(GLFW_KEY_O))
		{
			audioVolume += 0.01f;
			audioVolume = audioVolume < 0 ? 0 : audioVolume;
			//Audio::GetInstance().SetGroupVolume(ChannelGroups::MENUAUDIO, audioVolume);
		}
		

		//test cloning
		if (UME::Input::IsKeyPressed(GLFW_KEY_C))
		{
			GameObject Clone = GameObjectFactory::CloneObject(player_obj);
		}
	}

	void Level1_Draw()//rendering of the game for Level1
	{
		//some code here
		//std::cout << "Level1:Draw" << '\n';

		// Render the entities
		ECS::GetInstance().GetSystem<Renderer>()->render();
	}

	void Level1_Free()//release unused assets/variable memories
	{
		//some code here
		//std::cout << "Level1:Free" << '\n';
	}

	void Level1_Unload()//unload the level
	{
		//some code here
		//std::cout << "Level1:Unload" << '\n';

		// Unload the renderer system
		ECS::GetInstance().GetSystem<Renderer>()->cleanUp();
		Audio::GetInstance().StopAllSoundsInGroup(ChannelGroups::LEVEL1);
	}
}