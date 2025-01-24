/* Start Header ************************************************************************/
/*!
\file       DungeonManager.cpp
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Nov 24, 2024
\brief      This file contains the definition of the DungeonManager which handles the game dungeon.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "DungeonManager.h"				  // for forward declaration
#include "../Graphics/Camera2D.h"		  // for camera position
#include "../Factory/GameObjectManager.h" // for game object name and tag
#include "../Game/EnemyManager.h"		  // for updating enemy list

namespace Ukemochi
{
	/*!***********************************************************************
	\brief
	 Initialize the DungeonManager.
	*************************************************************************/
	void DungeonManager::Init()
	{
		//change when proper room structure is done
		current_room_id = 2;
		//current_room_wave = WAVE_NUMBER;
		player = static_cast<EntityID>(-1);

		InitDungeon();
	}

	/*!***********************************************************************
	\brief
	 Offset the position of a specific room and its entities.
	\param[in] room_id
	 The ID of the room to offset.
	\param[in] offset
	 The amount to offset the room by.
	*************************************************************************/
	void DungeonManager::OffsetRoom(const int room_id, const int offset)
	{
		std::string str_id = std::to_string(room_id);

		// Offset the room and its entity positions
		for (auto& entity : rooms[room_id].entities)
		{
			std::string name = GameObjectManager::GetInstance().GetGO(entity)->GetName();
			auto& transform = ECS::GetInstance().GetComponent<Transform>(entity);

			transform.position.x += offset;

			if (name == str_id + "_Background")
				rooms[room_id].position = Vec2(transform.position.x,transform.position.y);
		}
	}

	/*!***********************************************************************
	\brief
	 Switch the active room to the next room.
	\param[in] next_room_id
	 The ID of the room to switch to.
	*************************************************************************/
	void DungeonManager::SwitchToRoom(int next_room_id)
	{
		// Deactivate current room
		ActivateRoom(current_room_id, false);

		// Update current room ID
		current_room_id += next_room_id;

		// Perform camera and player transition
		if (next_room_id == -1)
		{
			//ECS::GetInstance().GetSystem<Camera>()->position.x -= ROOM_WIDTH;
			ECS::GetInstance().GetSystem<Camera>()->position.x = rooms[current_room_id].position.x - ROOM_WIDTH * 0.5f;

			auto& transform = ECS::GetInstance().GetComponent<Transform>(player);
			transform.position.x = rooms[current_room_id].position.x + PLAYER_OFFSET;
		}
		else
		{
			//ECS::GetInstance().GetSystem<Camera>()->position.x += ROOM_WIDTH;
			ECS::GetInstance().GetSystem<Camera>()->position.x = rooms[current_room_id].position.x - ROOM_WIDTH * 0.5f;

			auto& transform = ECS::GetInstance().GetComponent<Transform>(player);
			transform.position.x = rooms[current_room_id].position.x - PLAYER_OFFSET;
		}

		// Activate next room
		ActivateRoom(current_room_id, true);
	}

	/*!***********************************************************************
	\brief
	 Update the room's progress, such as unlocking doors upon completion.
	*************************************************************************/
	void DungeonManager::UpdateRoomProgress()
	{
		// Check if all enemies in the room is gone
		//if (rooms[current_room_id].enemies.size() <= 0)
		//	UnlockRoom();

		//bool kill_tracker = false; // kill tracker to trigger next wave. Need to link with EnemuManager? to read when enemy is defeated

		//if kill tracker toggles to true decrement wave number then reset to false;
		//if mobs in wave reachs 0 -> trigger some way to spawn next wave(need to find/ create spawning logic)

		for (auto enemy = rooms[current_room_id].enemies.begin(); enemy != rooms[current_room_id].enemies.end(); enemy++)
		{
			GameObject* enemyObj = GameObjectManager::GetInstance().GetGO(*enemy);
			auto& isDead = enemyObj->GetComponent<Enemy>();
			if (isDead.state == Enemy::DEAD)
			{
				rooms[current_room_id].enemies.erase(enemy);
			}
		}

		//hopefully unlock room will just trigger once
		if (rooms[current_room_id].enemies.empty() && !rooms[current_room_id].cleared)
		{
			rooms[current_room_id].cleared = true;
			UnlockRoom();
		}

	}

	/*!***********************************************************************
	\brief
	 Initialize the rooms in the dungeon.
	*************************************************************************/
	void DungeonManager::InitDungeon()
	{
		// Clear the rooms list
		rooms.clear();

		for (int room_id = 1; room_id <= NUM_OF_ROOMS; ++room_id)
		{
			std::string str_id = std::to_string(room_id);

			for (auto const& entity : m_Entities)
			{
				std::string name = GameObjectManager::GetInstance().GetGO(entity)->GetName();
				std::string tag = GameObjectManager::GetInstance().GetGO(entity)->GetTag();

				// Get the player entity
				if (player == -1 && tag == "Player")
					player = entity;

				// Store the entities in its own room
				if (name[0] == str_id[0])
				{
					// Store all entities of the same room in the room entity list
					rooms[room_id].entities.push_back(entity);

					if (tag == "Enemy")
					{
						// Store all enemies of the same room in the room enemy list
						rooms[room_id].enemies.push_back(entity);

						////do-while loop to handle multiple waves
						//do						
						//{
						//	if (rooms[room_id].mobs_in_wave[current_room_wave] < MAX_WAVE_SIZE)
						//	{
						//		rooms[room_id].mobs_in_wave[current_room_wave]++;
						//		//main goal is just to track addition correctly, leave the loop after successful addition;
						//		break;
						//	}
						//	else
						//	{
						//		//threshold is hit for the wave, go to the next wave and restart the loop
						//		current_room_wave++;
						//		continue;
						//	}
						//} while (rooms[room_id].mobs_in_wave[current_room_wave] < MAX_WAVE_SIZE);
						//	
						//
						//current_room_wave = WAVE_NUMBER;
					}
					else if (tag == "Room")
					{
						// Store the position of the room	
						auto& transform = ECS::GetInstance().GetComponent<Transform>(entity);
						rooms[room_id].position = Vec2(transform.position.x,transform.position.y);
					}
				}
			}

			// Deactivate all rooms except the current room
			if (room_id != current_room_id)
				ActivateRoom(room_id, false);
		}

		// Set the camera initial position
		ECS::GetInstance().GetSystem<Camera>()->position = {};
	}

	/*!***********************************************************************
	\brief
	 Activate or deactivate all entities in a room.
	\param[in] room_id
	 The ID of the room to activate or deactivate.
	\param[in] activate
	 True to activate the room, false to deactivate it.
	*************************************************************************/
	void DungeonManager::ActivateRoom(int room_id, bool activate)
	{
		for (auto& entity : rooms[room_id].entities)
			GameObjectManager::GetInstance().GetGO(entity)->SetActive(activate);

		if (!rooms[room_id].cleared)
		{
			ECS::GetInstance().GetSystem<EnemyManager>()->UpdateEnemyList();
		}
	}

	/*!***********************************************************************
	\brief
	 Unlock the room by enabling doors and disabling blocks.
	*************************************************************************/
	void DungeonManager::UnlockRoom()
	{
		//check for enemies


		std::string str_id = std::to_string(current_room_id);

		// Enable doors and disable blocks in the room
		for (auto const& entity : rooms[current_room_id].entities)
		{
			std::string name = GameObjectManager::GetInstance().GetGO(entity)->GetName();
			std::string tag = GameObjectManager::GetInstance().GetGO(entity)->GetTag();

			if(rooms[current_room_id].enemies.empty())
			{
				// Enable doors
				if (tag == "LeftDoor" || tag == "RightDoor")
					GameObjectManager::GetInstance().GetGO(entity)->SetActive(true);

				// Disable blocks
				if (name == str_id + "_LeftBlock" || name == str_id + "_RightBlock")
					GameObjectManager::GetInstance().GetGO(entity)->SetActive(false);
			}
		}
	}
}