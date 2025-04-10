/* Start Header ************************************************************************/
/*!
\file       DungeonManager.h
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Mar 27, 2025
\brief      This file contains the declaration of the DungeonManager which handles the game dungeon.

Copyright (C) 2025 DigiPen Institute of Technology.
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
	const int ROOM_WIDTH = 1920, ROOM_HEIGHT = 1080, PLAYER_OFFSET = 800;
	const int NUM_OF_ROOMS = 6;

	/*!***********************************************************************
	\brief
	 Structure to represent a room in the dungeon.
	*************************************************************************/
	struct Room
	{
		int id;
		std::vector<EntityID> entities;	  // Entities in the room
		std::vector<EntityID> enemies;	  // Enemies in the room
		size_t enemy_count;				  // Number of enemies in the room
		Vec2 position;					  // Position of the room
	};

	class DungeonManager : public System
	{
	public:
		EntityID player;					 // ID of the player entity
		std::unordered_map<int, Room> rooms; // Map of room IDs to room data
		int current_room_id;				 // ID of the current active room
		bool enemy_alive = false;			 // Flag to indicate if there are any enemies in the room left
		bool start_boss = false;			 // Flag to indicate boss fight has started
		bool end_boss = false;				 // Flag to indicate boss fight has ended

		/*!***********************************************************************
		\brief
		 Initialize the DungeonManager.
		*************************************************************************/
		void Init();

		/*!***********************************************************************
		\brief
		 Offset the position of a specific room and its entities.
		\param[in] room_id
		 The ID of the room to offset.
		\param[in] offset
		 The amount to offset the room by.
		*************************************************************************/
		void OffsetRoom(const int room_id, const int offset);

		/*!***********************************************************************
		\brief
		 Switch the active room to the next room.
		\param[in] next_room_id
		 The ID of the room to switch to.
		*************************************************************************/
		void SwitchToRoom(const int next_room_id);

		/*!***********************************************************************
		\brief
		 Update the room's progress, such as unlocking doors upon completion.
		*************************************************************************/
		void UpdateRoomProgress();

	private:
		/*!***********************************************************************
		\brief
		 Initialize the rooms in the dungeon.
		*************************************************************************/
		void InitDungeon();

		/*!***********************************************************************
		\brief
		 Activate or deactivate all entities in a room.
		\param[in] room_id
		 The ID of the room to activate or deactivate.
		\param[in] activate
		 True to activate the room, false to deactivate it.
		*************************************************************************/
		void ActivateRoom(const int room_id, const bool activate);

		/*!***********************************************************************
		\brief
		 Unlock the room by enabling doors and disabling blocks.
		*************************************************************************/
		void UnlockRoom();

		/*!***********************************************************************
		\brief
		 Reset the player's stats when exiting the tutorial room.
		*************************************************************************/
		void ExitTutorialRoom();
	};
}