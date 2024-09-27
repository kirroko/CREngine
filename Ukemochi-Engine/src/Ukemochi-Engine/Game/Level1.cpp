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

namespace Ukemochi
{
	// --- TEMP player variables ---
	EntityID player_entity;

	void Level1_Load()//Load all necessary assets before start of Level1
	{
		//some code here
		//std::cout << "Level1:Load" << '\n';
	}

	void Level1_Initialize()//Initialize the game at the start of Level1
	{
		//some code here
		//std::cout << "Level1:Initialize" << '\n';

		// Initialize the graphics system
		ECS::GetInstance().GetSystem<Renderer>()->init();

		// Create player entity
		player_entity = ECS::GetInstance().CreateEntity();
		ECS::GetInstance().AddComponent(player_entity, Transform{
				Vec2{ECS::GetInstance().GetSystem<Renderer>()->screen_width * 0.5f,
				ECS::GetInstance().GetSystem<Renderer>()->screen_height * 0.5f},
				Vec2{},
				Vec2{100.f, 100.f}
			});
		ECS::GetInstance().AddComponent(player_entity, Rigidbody2D{ Vec2{}, Vec2{250.f, 250.f} });
		ECS::GetInstance().AddComponent(player_entity, BoxCollider2D{});
		ECS::GetInstance().AddComponent(player_entity, SpriteRender{
				"../Assets/Textures/container.jpg",
				SPRITE_SHAPE::BOX,
				true,
				1.0f
			});

		//std::default_random_engine generator;
		//std::uniform_real_distribution<float> randPosition(450.0f, 800.0f);
		//std::uniform_real_distribution<float> randRotation(0.0f, 3.0f);
		//std::uniform_real_distribution<float> randScale(100.0f, 500.0f);

		//float scale = randScale(generator);

		//// Create entities
		//for (auto& entity : entities)
		//{
		//	entity = ECS::GetInstance().CreateEntity();
		//	ECS::GetInstance().AddComponent(entity, Transform{
		//		Vec2(randPosition(generator),randPosition(generator)),
		//		Vec2(randRotation(generator),randRotation(generator)),
		//		Vec2(scale,scale)
		//		});
		//	ECS::GetInstance().AddComponent(entity, Rigidbody2D());
		//	ECS::GetInstance().AddComponent(entity, SpriteRender{
		//		"../Assets/Textures/container.jpg",
		//		SPRITE_SHAPE::BOX,
		//		true,
		//		1.0f
		//		});
		//}
	}

	void Level1_Update()//Level1 game runtime
	{
		//some code here
		//std::cout << "Level1:Update" << '\n';

		// --- Handle User Input for Player controls ---
		// Press 'W' or up key to move the player up
		if (UME::Input::IsKeyPressed(UME_KEY_W) || UME::Input::IsKeyPressed(UME_KEY_UP))
		{
			auto& player_rb = ECS::GetInstance().GetComponent<Rigidbody2D>(player_entity);
			player_rb.velocity.y = -player_rb.acceleration.y;

			//temp
			auto& player_trans = ECS::GetInstance().GetComponent<Transform>(player_entity);
			std::cout << "X: " << (int)player_trans.position.x << ", Y: " << (int)player_trans.position.y << '\n';
		}
		// Press 'S' or down key to move the player down
		else if (UME::Input::IsKeyPressed(UME_KEY_S) || UME::Input::IsKeyPressed(UME_KEY_DOWN))
		{
			auto& player_rb = ECS::GetInstance().GetComponent<Rigidbody2D>(player_entity);
			player_rb.velocity.y = player_rb.acceleration.y;

			//temp
			auto& player_trans = ECS::GetInstance().GetComponent<Transform>(player_entity);
			std::cout << "X: " << (int)player_trans.position.x << ", Y: " << (int)player_trans.position.y << '\n';
		}
		else
		{
			auto& player_rb = ECS::GetInstance().GetComponent<Rigidbody2D>(player_entity);
			player_rb.velocity.y = 0.0f; // Stop moving the player in the y axis
		}

		// Press 'A' or left key to move the player left
		if (UME::Input::IsKeyPressed(UME_KEY_A) || UME::Input::IsKeyPressed(UME_KEY_LEFT))
		{
			auto& player_rb = ECS::GetInstance().GetComponent<Rigidbody2D>(player_entity);
			player_rb.velocity.x = -player_rb.acceleration.x;

			//temp
			auto& player_trans = ECS::GetInstance().GetComponent<Transform>(player_entity);
			std::cout << "X: " << (int)player_trans.position.x << ", Y: " << (int)player_trans.position.y << '\n';
		}
		// Press 'D' or right key to move the player to the right
		else if (UME::Input::IsKeyPressed(UME_KEY_D) || UME::Input::IsKeyPressed(UME_KEY_RIGHT))
		{
			auto& player_rb = ECS::GetInstance().GetComponent<Rigidbody2D>(player_entity);
			player_rb.velocity.x = player_rb.acceleration.x;

			//temp
			auto& player_trans = ECS::GetInstance().GetComponent<Transform>(player_entity);
			std::cout << "X: " << (int)player_trans.position.x << ", Y: " << (int)player_trans.position.y << '\n';
		}
		else
		{
			auto& player_rb = ECS::GetInstance().GetComponent<Rigidbody2D>(player_entity);
			player_rb.velocity.x = 0.0f; // Stop moving the player in the x axis
		}
		// --- End User Input ---

		// Update the entities physics
		ECS::GetInstance().GetSystem<Physics>()->UpdatePhysics();

		// Check collisions between the entities
		ECS::GetInstance().GetSystem<Collision>()->CheckCollisions();
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
	}
}