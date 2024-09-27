/* Start Header ************************************************************************/
/*!
\file    Level1.cpp
\author  Tan Si Han, t.sihan, 2301264
\par     t.sihan@digipen.edu
\date    20/09/2024
\brief   This files contains the definction of functions in Level1.


Copyright (C) 2024 DigiPen Institute of Technology.  Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#include "PreCompile.h"

#include "GSM.h"
#include "Level1.h"
#include <iostream>

#include "../Input.h"					// for user input
#include "../Collision/Collision.h"		// for collision system
#include "../Physics/Physics.h"			// for physics system
//#include "../Graphics/Renderer.h"		// for render system

#include "../ECS/Components.h" // for Rigidbody2D, BoxCollider2D component

// --- TEMP player variables ---
Ukemochi::BoxCollider2D player_box_collider;
Ukemochi::Rigidbody2D player_rb;
//Renderer renderer_system;

void Level1_Load()//Load all necessary assets before start of Level1
{
	//some code here
	//std::cout << "Level1:Load" << '\n';
}

void Level1_Initialize()//Initialize the game at the start of Level1
{
	//some code here
	//std::cout << "Level1:Initialize" << '\n';
}

void Level1_Update()//Level1 game runtime
{
	//some code here
	//std::cout << "Level1:Update" << '\n';

	// --- Handle User Input for Player controls ---
	// Press 'W' or up key to move the player up
	if (UME::Input::IsKeyPressed(UME_KEY_W) || UME::Input::IsKeyPressed(UME_KEY_UP))
	{
		player_rb.velocity.y = -player_rb.acceleration.y;
		std::cout << "X: " << (int)player_rb.position_curr.x << ", Y: " << (int)player_rb.position_curr.y << '\n';
	}
	// Press 'S' or down key to move the player down
	else if (UME::Input::IsKeyPressed(UME_KEY_S) || UME::Input::IsKeyPressed(UME_KEY_DOWN))
	{
		player_rb.velocity.y = player_rb.acceleration.y;
		std::cout << "X: " << (int)player_rb.position_curr.x << ", Y: " << (int)player_rb.position_curr.y << '\n';
	}
	else
		player_rb.velocity.y = 0.0f; // Stop moving the player in the y axis

	// Press 'A' or left key to move the player left
	if (UME::Input::IsKeyPressed(UME_KEY_A) || UME::Input::IsKeyPressed(UME_KEY_LEFT))
	{
		player_rb.velocity.x = -player_rb.acceleration.x;
		std::cout << "X: " << (int)player_rb.position_curr.x << ", Y: " << (int)player_rb.position_curr.y << '\n';
	}
	// Press 'D' or right key to move the player to the right
	else if (UME::Input::IsKeyPressed(UME_KEY_D) || UME::Input::IsKeyPressed(UME_KEY_RIGHT))
	{
		player_rb.velocity.x = player_rb.acceleration.x;
		std::cout << "X: " << (int)player_rb.position_curr.x << ", Y: " << (int)player_rb.position_curr.y << '\n';
	}
	else
		player_rb.velocity.x = 0.0f; // Stop moving the player in the x axis
	// --- End User Input ---

	// Update player rb and box collider
	Ukemochi::Physics::UpdatePhysics(player_rb, player_box_collider);

	// Check collisions
	float tLast;
	if (Ukemochi::Collision::CollisionIntersection_BoxBox(player_box_collider, player_rb.velocity, player_box_collider, player_rb.velocity, tLast))
	{
		// do collision response
		//player_rb.position = player_rb.initial_position;
		//std::cout << "box box collided\n";
	}
	//if (Ukemochi::Collision::CollisionIntersection_BoxScreen(player_box_collider, renderer_system.screen_width, renderer_system.screen_height))
	//{
	//	//std::cout << "box collided with game screen boundary\n";
	//}
}

void Level1_Draw()//rendering of the game for Level1
{
	//some code here
	//std::cout << "Level1:Draw" << '\n';
	//renderer_system.drawBox(player_rb.position_curr.x + renderer_system.screen_width / 2, player_rb.position_curr.y + renderer_system.screen_height / 2, player_rb.scale.x, player_rb.scale.y, 0);
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
}