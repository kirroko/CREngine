/* Start Header ************************************************************************/
/*!
\file       DungeonManager.cpp
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu (75%)
\co-author	Pek Jun Kai Gerald, p.junkaigerald, 2301334, p.junkaigerald\@digipen.edu (25%)
\date       Mar 27, 2025
\brief      This file contains the definition of the DungeonManager which handles the game dungeon.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "DungeonManager.h"				  // for forward declaration
#include "../Graphics/Camera2D.h"		  // for camera position
#include "../Factory/GameObjectManager.h" // for game object name and tag
#include "../Game/EnemyManager.h"		  // for updating enemy list
#include "../Game/BossManager.h"		  // for init boss
#include "../SceneManager.h"			  // for GetCurrScene name
#include "../Video/VideoManager.h"		  // for boss cutscene
#include "Ukemochi-Engine/Application.h"  // for pause

namespace Ukemochi
{
	/*!***********************************************************************
	\brief
	 Initialize the DungeonManager.
	*************************************************************************/
	void DungeonManager::Init()
	{
		current_room_id = 1;
		player = static_cast<EntityID>(-1);

		start_boss = false;
		end_boss = false;

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
				rooms[room_id].position = Vec2(transform.position.x, transform.position.y);
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

		// Reset the soul bars and charges when exiting the tutorial room (room 1)
		if (current_room_id == 1)
			ExitTutorialRoom();

		// Update current room ID
		current_room_id += next_room_id;

		// Perform camera and player transition
		if (next_room_id == -1)
		{
			// Set camera position to new room position
			ECS::GetInstance().GetSystem<Camera>()->position.x = rooms[current_room_id].position.x - ROOM_WIDTH * 0.5f;

			// Set player position to new room position
			auto& transform = ECS::GetInstance().GetComponent<Transform>(player);
			if (current_room_id == 6)
				transform.position.x = rooms[current_room_id].position.x;
			else
				transform.position.x = rooms[current_room_id].position.x + PLAYER_OFFSET;

			// Set red and blue soul positions to new room position
			if (GameObjectManager::GetInstance().GetGOByTag("UI_Blue_Soul"))
			{
				auto& blue_soul_transform = GameObjectManager::GetInstance().GetGOByTag("UI_Blue_Soul")->GetComponent<Transform>();
				blue_soul_transform.position.x -= ROOM_WIDTH;
			}
			if (GameObjectManager::GetInstance().GetGOByTag("UI_Red_Soul"))
			{
				auto& red_soul_transform = GameObjectManager::GetInstance().GetGOByTag("UI_Red_Soul")->GetComponent<Transform>();
				red_soul_transform.position.x -= ROOM_WIDTH;
			}
		}
		else
		{
			// Set camera position to new room position
			ECS::GetInstance().GetSystem<Camera>()->position.x = rooms[current_room_id].position.x - ROOM_WIDTH * 0.5f;

			// Set player position to new room position
			auto& transform = ECS::GetInstance().GetComponent<Transform>(player);
			if (current_room_id == 6)
				transform.position.x = rooms[current_room_id].position.x;
			else
				transform.position.x = rooms[current_room_id].position.x - PLAYER_OFFSET;

			// Set red and blue soul positions to new room position
			if (GameObjectManager::GetInstance().GetGOByTag("UI_Blue_Soul"))
			{
				auto& blue_soul_transform = GameObjectManager::GetInstance().GetGOByTag("UI_Blue_Soul")->GetComponent<Transform>();
				blue_soul_transform.position.x += ROOM_WIDTH;
			}
			if (GameObjectManager::GetInstance().GetGOByTag("UI_Red_Soul"))
			{
				auto& red_soul_transform = GameObjectManager::GetInstance().GetGOByTag("UI_Red_Soul")->GetComponent<Transform>();
				red_soul_transform.position.x += ROOM_WIDTH;
			}
		}

		// Load before boss cutscene when entering the boss room (room 6)
#ifndef _DEBUG
		if (current_room_id == 6)
		{
			ECS::GetInstance().GetSystem<Camera>()->position = { 0, 0 };
			if (ECS::GetInstance().GetSystem<VideoManager>()->videos["before_boss"].loaded)
			{
				ECS::GetInstance().GetSystem<VideoManager>()->SetCurrentVideo("before_boss");
				ECS::GetInstance().GetSystem<VideoManager>()->videos["before_boss"].currentFrame = 0;
				ECS::GetInstance().GetSystem<VideoManager>()->videos["before_boss"].done = false;
			}
			else
			{
				Application::Get().SetPaused(true);
				ECS::GetInstance().GetSystem<VideoManager>()->SetCurrentVideo("loading");
				ECS::GetInstance().GetSystem<VideoManager>()->videos["loading"].currentFrame = 0;
				ECS::GetInstance().GetSystem<VideoManager>()->videos["loading"].done = false;
			}
		}
#else
		if (current_room_id == 6) // Skip cutscene in debug mode
		{
			// Init boss
			ECS::GetInstance().GetSystem<BossManager>()->InitBoss();
			ECS::GetInstance().GetSystem<BossManager>()->SetBossPhase(1);
			ECS::GetInstance().GetSystem<EnemyManager>()->numEnemyTarget = 0;

			// Play boss enter SFX
			if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
			{
				auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
				audioM.StopMusic(audioM.GetMusicIndex("PreBoss_BGM"));
				// Play BGM at 0.2 volume
				if (audioM.GetMusicIndex("BGM") != -1)
				{
					int bgmIndex = audioM.GetMusicIndex("BGM");
					if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsMusicPlaying(bgmIndex))
					{
						audioM.PlayMusic(bgmIndex);
						// Set volume for BGM
						ECS::GetInstance().GetSystem<Audio>()->GetInstance().SetAudioVolume(bgmIndex, 0.2f, "Music");
					}
				}

				if (audioM.GetSFXindex("LevelChange") != -1)
				{
					if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("LevelChange")))
						audioM.PlaySFX(audioM.GetSFXindex("LevelChange"));
				}
			}

			start_boss = true;
		}
#endif

		// Activate current room
		ActivateRoom(current_room_id, true);
	}

	/*!***********************************************************************
	\brief
	 Update the room's progress, such as unlocking doors upon completion.
	*************************************************************************/
	void DungeonManager::UpdateRoomProgress()
	{
		// Check if all enemies in the room is active
		enemy_alive = false;
		for (auto enemy = rooms[current_room_id].enemies.begin(); enemy != rooms[current_room_id].enemies.end(); enemy++)
		{
			GameObject* enemyObj = GameObjectManager::GetInstance().GetGO(*enemy);

			if (enemyObj->GetActive())
				enemy_alive = true;
		}

		// Unlock the room if all enemies are not active
		if (!enemy_alive)
			UnlockRoom();

#ifndef _DEBUG
		// Move camera back to boss room and start boss fight
		if (current_room_id == 6 && !start_boss && ECS::GetInstance().GetSystem<VideoManager>()->IsVideoDonePlaying("before_boss"))
		{
			//ECS::GetInstance().GetSystem<VideoManager>()->SetCurrentVideo("before_boss"); // Select just in case
			//ECS::GetInstance().GetSystem<VideoManager>()->Free(); // Free current video

			// Set camera to room position
			ECS::GetInstance().GetSystem<Camera>()->position.x = rooms[current_room_id].position.x - ROOM_WIDTH * 0.5f;
			
			// Init boss
			ECS::GetInstance().GetSystem<BossManager>()->InitBoss();
			ECS::GetInstance().GetSystem<BossManager>()->SetBossPhase(1);
			ECS::GetInstance().GetSystem<EnemyManager>()->numEnemyTarget = 0;

			// Play boss enter SFX
			if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
			{
				auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
				audioM.StopMusic(audioM.GetMusicIndex("PreBoss_BGM"));
				// Play BGM at 0.2 volume
				if (audioM.GetMusicIndex("BGM") != -1)
				{
					int bgmIndex = audioM.GetMusicIndex("BGM");
					if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsMusicPlaying(bgmIndex))
					{
						audioM.PlayMusic(bgmIndex);
						// Set volume for BGM
						ECS::GetInstance().GetSystem<Audio>()->GetInstance().SetAudioVolume(bgmIndex, 0.2f, "Music");
					}
				}

				if (audioM.GetSFXindex("LevelChange") != -1)
				{
					if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("LevelChange")))
						audioM.PlaySFX(audioM.GetSFXindex("LevelChange"));
				}
			}

			start_boss = true;
		}

		// What happens after boss fight
		if (current_room_id == 6 && !end_boss && ECS::GetInstance().GetSystem<BossManager>()->GetBossPhase() == 3
			&& ECS::GetInstance().GetSystem<VideoManager>()->IsVideoDonePlaying("after_boss"))
		{
			if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
			{
				auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
				audioM.StopMusic(audioM.GetMusicIndex("AfterBoss_BGM"));
			}

			Application::Get().SetPaused(true);
			Application::Get().GameStarted = false;

			ECS::GetInstance().GetSystem<VideoManager>()->videos["main_menu"].done = false;
			ECS::GetInstance().GetSystem<VideoManager>()->SetCurrentVideo("main_menu");
			ECS::GetInstance().GetSystem<Camera>()->position = { 0,0 };
			ECS::GetInstance().GetSystem<InGameGUI>()->RemoveGameUI();
			ECS::GetInstance().GetSystem<InGameGUI>()->CreateMainMenuUI();
			ECS::GetInstance().GetSystem<InGameGUI>()->ShowCredits();
			ECS::GetInstance().GetSystem<InGameGUI>()->showCredits = true;

			end_boss = true;
		}
#else
		if (current_room_id == 6 && !end_boss && ECS::GetInstance().GetSystem<BossManager>()->GetBossPhase() == 3)
		{
			if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
			{
				auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
				audioM.StopMusic(audioM.GetMusicIndex("AfterBoss_BGM"));
			}

			Application::Get().SetPaused(true);
			Application::Get().GameStarted = false;
			end_boss = true;
		}
#endif

		////healing to post injuries max health here
		//if (player != -1)
		//{
		//	while (GameObjectManager::GetInstance().GetGO(player)->GetComponent<Player>().currentHealth < GameObjectManager::GetInstance().GetGO(player)->GetComponent<Player>().postInjuriesMaxHealth && current_room_id != 1)
		//	{
		//		GameObjectManager::GetInstance().GetGO(player)->GetComponent<Player>().currentHealth += 2;
		//		if (GameObjectManager::GetInstance().GetGO(player)->GetComponent<Player>().currentHealth > GameObjectManager::GetInstance().GetGO(player)->GetComponent<Player>().postInjuriesMaxHealth)
		//		{
		//			GameObjectManager::GetInstance().GetGO(player)->GetComponent<Player>().currentHealth = GameObjectManager::GetInstance().GetGO(player)->GetComponent<Player>().postInjuriesMaxHealth;
		//		}
		//	}
		//}
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
					}
					else if (tag == "Room")
					{
						// Store the position of the room	
						auto& transform = ECS::GetInstance().GetComponent<Transform>(entity);
						rooms[room_id].position = Vec2(transform.position.x, transform.position.y);
					}
				}
			}

			// Storing enemy count to check whether room is cleared (So that door remains open as to allow backtracking)
			rooms[room_id].enemy_count = rooms[room_id].enemies.size();

			// Deactivate all rooms except the current room
			if (room_id != current_room_id)
				ActivateRoom(room_id, false);
			else
				ActivateRoom(room_id, true);
		}

		// Set the camera initial position
		if (SceneManager::GetInstance().GetCurrScene() == "ALevel1")
			ECS::GetInstance().GetSystem<Camera>()->position = { -ROOM_WIDTH, 0 };
		else
			ECS::GetInstance().GetSystem<Camera>()->position = { 0, 0 };
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
		{
			// Dont activate enemy if its dead
			if (GameObjectManager::GetInstance().GetGO(entity)->GetTag() == "Enemy")
			{
				auto& enemy = ECS::GetInstance().GetComponent<Enemy>(entity);
				if (enemy.health <= 0)
				{
					GameObjectManager::GetInstance().GetGO(entity)->SetActive(false);
					GameObjectManager::GetInstance().GetGOByName(GameObjectManager::GetInstance().GetGO(entity)->GetName() + "_Shadow")->SetActive(false);
					continue;
				}
			}

			GameObjectManager::GetInstance().GetGO(entity)->SetActive(activate);
			std::string room = std::to_string(room_id);
			std::string name = GameObjectManager::GetInstance().GetGO(entity)->GetName();

			// Checking current enemy count in the room
			if (rooms[room_id].enemy_count > 0)
			{
				// If room has enemies doors are disabled
				if (name == room + "_LeftDoor" || name == room + "_RightDoor")
					GameObjectManager::GetInstance().GetGO(entity)->SetActive(false);
			}
			else
			{
				// Vice versa if there are no enemies doors are enabled
				if (name == room + "_LeftDoor" || name == room + "_RightDoor")
					GameObjectManager::GetInstance().GetGO(entity)->SetActive(true);
			}
		}

		ECS::GetInstance().GetSystem<EnemyManager>()->UpdateEnemyList();
	}

	/*!***********************************************************************
	\brief
	 Unlock the room by enabling doors and disabling blocks.
	*************************************************************************/
	void DungeonManager::UnlockRoom()
	{
		std::string str_id = std::to_string(current_room_id);

		// Enable doors and disable blocks in the room
		for (auto const& entity : rooms[current_room_id].entities)
		{
			std::string name = GameObjectManager::GetInstance().GetGO(entity)->GetName();
			std::string tag = GameObjectManager::GetInstance().GetGO(entity)->GetTag();

			// Enable doors
			if (tag == "LeftDoor" || tag == "RightDoor")
				GameObjectManager::GetInstance().GetGO(entity)->SetActive(true);

			// Disable blocks
			if (name == str_id + "_LeftBlock" || name == str_id + "_RightBlock")
				GameObjectManager::GetInstance().GetGO(entity)->SetActive(false);
		}
	}

	/*!***********************************************************************
	\brief
	 Reset the player's stats when exiting the tutorial room.
	*************************************************************************/
	void DungeonManager::ExitTutorialRoom()
	{
		if (current_room_id == 1 && !SceneManager::GetInstance().enable_cheatmode) // Dont reset stats if cheat mode is enabled
		{
			auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);
			auto& player_animator = ECS::GetInstance().GetComponent<Animation>(player);

			for (int i = 0; i < NUM_OF_SOULS; i++)
			{
				player_soul.soul_bars[i] = 0.f;
				player_soul.soul_charges[i] = 0;

				// Disable red and blue souls
				if (GameObjectManager::GetInstance().GetGOByTag("UI_Red_Soul"))
					GameObjectManager::GetInstance().GetGOByTag("UI_Red_Soul")->SetActive(false);
				if (GameObjectManager::GetInstance().GetGOByTag("UI_Blue_Soul"))
					GameObjectManager::GetInstance().GetGOByTag("UI_Blue_Soul")->SetActive(false);

				// Currently in FISH soul, switch to EMPTY soul
				if (player_soul.current_soul == FISH && player_soul.current_soul == i)
				{
					player_soul.current_soul = EMPTY;
					player_animator.SetAnimationUninterrupted("SwitchBN");
					GameObjectManager::GetInstance().GetGOByTag("Soul")->SetActive(false);
				}
				// Currently in WORM soul, switch to EMPTY soul
				else if (player_soul.current_soul == WORM && player_soul.current_soul == i)
				{
					player_soul.current_soul = EMPTY;
					player_animator.SetAnimationUninterrupted("SwitchRN");
					GameObjectManager::GetInstance().GetGOByTag("Soul")->SetActive(false);
				}
			}
		}
	}
}
