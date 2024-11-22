/* Start Header ************************************************************************/
/*!
\file       DungeonManager.cpp
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Nov 21, 2024
\brief      This file contains the definition of the DungeonManager which handles the game dungeon.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "DungeonManager.h" // for forward declaration
#include "../Graphics/Camera2D.h"
#include "../Factory/GameObjectManager.h"

namespace Ukemochi
{
	void DungeonManager::InitDungeon()
	{
		current_room_id = 1;

		for (int room_id = 1; room_id <= NUM_OF_ROOMS; ++room_id)
		{
			// Store the entities in its own room
			for (auto const& entity : m_Entities)
			{
				std::string name = GameObjectManager::GetInstance().GetGO(entity)->GetName();
				//std::string tag = GameObjectManager::GetInstance().GetGO(entity)->GetTag();

				std::string str_id = std::to_string(room_id);
				if (name == "Room" + str_id
					|| name == "TopBound" + str_id || name == "BtmBound" + str_id || name == "LeftBound" + str_id || name == "RightBound" + str_id
					|| name == "LeftDoor" + str_id || name == "RightDoor" + str_id || name == "Box" + str_id || name == "Enemy" + str_id)
				{
					rooms[room_id].entities.push_back(entity);

					if (name == "Enemy" + str_id)
						rooms[room_id].enemies.push_back(entity);
				}
			}

			// Offset the room positions
			for (auto const& entity : rooms[room_id].entities)
			{
				std::string name = GameObjectManager::GetInstance().GetGO(entity)->GetName();
				std::string str_id = std::to_string(room_id);

				auto& transform = ECS::GetInstance().GetComponent<Transform>(entity);

				transform.position.x += ROOM_WIDTH * (room_id - 1);

				if (name == "Room" + str_id)
				{
					auto& transform = ECS::GetInstance().GetComponent<Transform>(entity);
					rooms[room_id].position = transform.position;
				}
			}
		}

		ECS::GetInstance().GetSystem<Camera>()->position = {};
	}

	void DungeonManager::Update()
	{

	}

	void DungeonManager::SwitchToRoom(int next_room_id)
	{
		//if (rooms.find(next_room_id) == rooms.end())
		//	return;

		// Deactivate current room
		ActivateRoom(current_room_id, false);

		// Update current room ID
		current_room_id += next_room_id;

		// Perform camera transition
		if (next_room_id == -1)
			ECS::GetInstance().GetSystem<Camera>()->position.x -= ROOM_WIDTH;
		else
			ECS::GetInstance().GetSystem<Camera>()->position.x += ROOM_WIDTH;

		//for (auto const& entity : m_Entities)
		//{
		//	if (GameObjectManager::GetInstance().GetGO(entity)->GetTag() == "Player")
		//	{

		//	}
		//}

		//ECS::GetInstance().GetSystem<Camera>()->position.x = rooms[current_room_id].position.x - ROOM_WIDTH;

		// Activate next room
		ActivateRoom(current_room_id, true);
	}

	void DungeonManager::ActivateRoom(int room_id, bool activate)
	{
		//auto& room = rooms[room_id];
		//room.is_active = activate;

		//for (auto& entity : room.entities)
		//{
		//	//auto& entityCollider = ECS::GetInstance().GetComponent<BoxCollider2D>(entity);
		//	//entityCollider.is_active = activate;

		//	//auto& entityRenderer = ECS::GetInstance().GetComponent<Renderer>(entity);
		//	//entityRenderer.is_visible = activate;
		//}
	}
}