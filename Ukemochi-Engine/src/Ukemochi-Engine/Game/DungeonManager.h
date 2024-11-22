/* Start Header ************************************************************************/
/*!
\file       DungeonManager.h
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Nov 21, 2024
\brief      This file contains the declaration of the DungeonManager which handles the game dungeon.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include "../ECS/ECS.h" // for ECS system and components

namespace Ukemochi
{
	/*!***********************************************************************
	\brief
	 Dungeon room constants.
	*************************************************************************/
	const int ROOM_WIDTH = 1920, ROOM_HEIGHT = 1080;
	const int NUM_OF_ROOMS = 3;

	struct Room
	{
		int id;
		std::vector<EntityID> entities;	  // Gameobjects in the room
		std::vector<EntityID> enemies;	  // Enemies in the room
		Vec2 position;					  // Position of the room
		bool is_active;					  // Is the room active?
		bool is_locked;					  // Is the room locked with enemies?
	};

	class DungeonManager : public System
	{
	private:
		std::unordered_map<int, Room> rooms;

	public:
		int current_room_id;

		void InitDungeon();

		void Update();

		void SwitchToRoom(int next_room_id);

		void ActivateRoom(int room_id, bool activate);
	};
}