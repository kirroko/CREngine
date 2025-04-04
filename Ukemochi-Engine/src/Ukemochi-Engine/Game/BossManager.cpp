/* Start Header ************************************************************************/
/*!
\file       BossManager.cpp
\author     Tan Si Han, t.sihan, 2301264, t.sihan@digipen.edu (90%)
\co-authors HURNG Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu (10%)
\date       Feb 05, 2025
\brief      This file contains the definition of the BossManager class and related methods.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#include "PreCompile.h"
#include "BossManager.h"
#include "../Factory/GameObjectManager.h"
#include "DungeonManager.h"
#include "../FrameController.h"
#include "../Game/EnemyManager.h"
#include "../Game/PlayerManager.h"
#include "../SceneManager.h"	   // for cheat mode
#include "../Graphics/Camera2D.h"  // for camera position
#include "../Video/VideoManager.h" // for boss cutscene

namespace Ukemochi
{
	void BossManager::InitBoss()
	{

		hair = GameObjectManager::GetInstance().GetGOByTag("Hair");
		if (hair && hair->HasComponent<Animation>())
		{
			hair->GetComponent<Animation>().SetAnimation("HairAtk");
		}
		if (hair && hair->HasComponent<SpriteRender>())
		{
			hairSR = GameObjectManager::GetInstance().GetGOByTag("Hair")->GetComponent<SpriteRender>();
		}

		hairHitBox = GameObjectManager::GetInstance().GetGOByTag("HitBox");
		if (hairHitBox)
		{
			hairPosX = hair->GetComponent<Transform>().position.x;
		}

		blob = GameObjectManager::GetInstance().GetGOByTag("BlobClone");
		if (blob && blob->HasComponent<Animation>())
		{
			blob->GetComponent<Animation>().SetAnimation("Idle");
			blobsize = blob->GetComponent<Transform>().scale.x;
			blob->GetComponent<Transform>().scale = Vec2();
		}

		spawnBlob = false;
		numOfBlob = 0;

		playerObj = GameObjectManager::GetInstance().GetGOByTag("Player");
		boss = GameObjectManager::GetInstance().GetGOByTag("Boss");
		if (boss && boss->HasComponent<Boss>())
		{
			bossCom = GameObjectManager::GetInstance().GetGOByTag("Boss")->GetComponent<Boss>();
			bossCom.health = 1000.f;
			bossCom.BossPhase = 0;
		}

		isHairAtk = true;
		atk = false;
	}
	void BossManager::UpdateBoss()
	{
		if (ECS::GetInstance().GetSystem<DungeonManager>()->current_room_id != 6)
			return;
		if (!boss || !playerObj)
		{
			return;
		}

		for (int step = 0; step < g_FrameRateController.GetCurrentNumberOfSteps(); ++step)
		{
			if (bossCom.BossPhase == 0)
			{
				if (hair)
				{
					hair->SetActive(false);
				}
				if (hairHitBox)
				{
					hairHitBox->SetActive(false);
				}
			}
			else if (bossCom.BossPhase == 1)
			{
				if (hair)
				{
					hair->SetActive(false);
				}
				if (hairHitBox)
				{
					hairHitBox->SetActive(false);
				}
				// CHANGE TO IF THERE NO ENEMY LEFT AND NUM OF BLOB MORE THAN 2
				if (!ECS::GetInstance().GetSystem<DungeonManager>()->enemy_alive && numOfBlob >= 4)
				{
					// play animation
					GameObjectManager::GetInstance().GetGOByTag("Boss")->GetComponent<Animation>().SetAnimationUninterrupted("Change");
					// animation finish then proceed
					bossCom.BossPhase = 2;
					bossCom.waitTime = 2.f;
				}
				else
				{
					Phase1();
				}
			}
			else if (bossCom.BossPhase == 2)
			{
				if (bossCom.health <= 0)
				{
					// WIN
					boss->SetActive(false);
					hair->SetActive(false);
					bossCom.BossPhase = 3;

					// Play after boss cutscene
					ECS::GetInstance().GetSystem<Camera>()->position = { 0, 0 };
					ECS::GetInstance().GetSystem<VideoManager>()->videos["after_boss"].done = false;
					ECS::GetInstance().GetSystem<VideoManager>()->SetCurrentVideo("after_boss");
					ECS::GetInstance().GetSystem<VideoManager>()->videos["after_boss"].currentFrame = 0;
				}
				else
				{
					ECS::GetInstance().GetSystem<Camera>()->UpdateShake(g_FrameRateController.GetFixedDeltaTime());
					Phase2();
				}
			}
			else if (bossCom.BossPhase == 3)
			{
				// After death phase
			}
		}
	}
	void BossManager::Phase1()
	{
		if (bossCom.BossPhase == 1)
		{
			GameObjectManager::GetInstance().GetGOByTag("Boss")->GetComponent<Animation>().SetAnimation("Idle1");
		}
		// attack
		// timer end play attack
		if (bossCom.waitTime < 5.f)
		{
			bossCom.waitTime += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
		}
		else
		{
			if (numOfBlob <= 3)
			{
				spawnBlob = true;
				// play animation
				GameObjectManager::GetInstance().GetGOByTag("Boss")->GetComponent<Animation>().SetAnimationUninterrupted("Change");
				numOfBlob++;
			}
		}
		// spawn blob blob
		// scale from 0 - orignal size
		if (spawnBlob && numOfBlob <= 3)
		{
			// spawn blob blob
			static int number = 0;
			GameObject* cloneObject = GameObjectManager::GetInstance().GetGOByTag("BlobClone");
			if (cloneObject != nullptr)
			{
				std::string name = "blob" + std::to_string(number++);
				GameObject& newObject = GameObjectManager::GetInstance().CloneObject(*cloneObject, name, "Blob");

				newObject.GetComponent<Transform>().position.x = playerObj->GetComponent<Transform>().position.x;
				newObject.GetComponent<Transform>().position.y = playerObj->GetComponent<Transform>().position.y;
			}
			spawnBlob = false;
			bossCom.waitTime = 0;
		}
	}
	void BossManager::Phase2()
	{
		if (hairSR.color.y == 0.f)
		{
			hairSR.color = Vec3(1.f, 1.f, 1.f);
		}

		GameObjectManager::GetInstance().GetGOByTag("Boss")->GetComponent<Animation>().SetAnimation("Idle2");
		static float delay = 0.f;
		static float atkdelay = 0.f;
		Phase1();
		if (numOfBlob >= 3)
		{
			numOfBlob = 0;
		}

		// hair attack
		if (delay < 6.f)
		{
			delay += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());

			if (delay > 5.f)
			{
				hair->SetActive(false);

				if (!hairHitBox->GetActive())
				{
					// set hair attack
					if (playerObj->GetComponent<Transform>().position.x > boss->GetComponent<Transform>().position.x + 50.f)
					{
						// right
						hair->GetComponent<SpriteRender>().flipX = true;
						hairHitBox->GetComponent<Transform>().scale.x = -853.000f;

						hair->GetComponent<Transform>().position.x = 9325;
						hairHitBox->GetComponent<Transform>().position.x = 9150;
						hairPosX = hair->GetComponent<Transform>().position.x;
					}
					else
					{
						// left
						hair->GetComponent<SpriteRender>().flipX = false;
						hairHitBox->GetComponent<Transform>().position.x = 8100;

						hair->GetComponent<Transform>().position.x = 7955;
						hairHitBox->GetComponent<Transform>().scale.x = 853.000f;
						hairPosX = hair->GetComponent<Transform>().position.x;
					}

					hair->GetComponent<Transform>().position.y = playerObj->GetComponent<Rigidbody2D>().position.y - 25.f;
					hairHitBox->GetComponent<Transform>().position.y = hair->GetComponent<Transform>().position.y - 75.f;
					hairHitBox->SetActive(true);
				}

			}
		}
		else
		{
			if (!atk)
			{
				hair->SetActive(true);
				hair->GetComponent<Animation>().RestartAnimation();
				hairSR.color = Vec3(1.f, 1.f, 1.f);

				atk = true;
				isHairAtk = false;
			}
		}

		if (boss->GetComponent<Animation>().GetCurrentFrame() == 15)
		{
			if (rand() % 100 < 30 && GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
			{
				auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
				// Play different hit sounds based on enemy type

				std::vector<int> bossAttackVOsounds = {
				audioM.GetSFXindex("BossAttackVO1"),
				audioM.GetSFXindex("BossAttackVO2"),
				audioM.GetSFXindex("BossAttackVO3"),
				audioM.GetSFXindex("BossAttackVO4"),
				audioM.GetSFXindex("BossAttackVO5")
				};

				// Remove invalid (-1) sounds
				bossAttackVOsounds.erase(
					std::remove(bossAttackVOsounds.begin(), bossAttackVOsounds.end(), -1),
					bossAttackVOsounds.end()
				);

				// If no valid sounds found, try to use the original as fallback
				if (bossAttackVOsounds.empty()) {
					int bossAttackVOOriginal = audioM.GetSFXindex("BossAttack1");
					if (bossAttackVOOriginal != -1) {
						bossAttackVOsounds.push_back(bossAttackVOOriginal);
					}
				}

				// Keep track of which sounds have been played (static at class level)
				static std::vector<bool> bossAttackVOSoundsPlayed;

				// Initialize if first time or size changed
				if (bossAttackVOSoundsPlayed.size() != bossAttackVOsounds.size()) {
					bossAttackVOSoundsPlayed.resize(bossAttackVOsounds.size(), false);
				}

				// Check if all sounds have been played
				bool allPlayed = true;
				for (bool played : bossAttackVOSoundsPlayed) {
					if (!played) {
						allPlayed = false;
						break;
					}
				}

				// If all sounds have been played, reset all to unplayed
				if (allPlayed) {
					std::fill(bossAttackVOSoundsPlayed.begin(), bossAttackVOSoundsPlayed.end(), false);
				}

				// Get sounds that haven't been played yet
				std::vector<int> availableSoundIndices;
				for (int i = 0; i < bossAttackVOsounds.size(); i++) {
					if (!bossAttackVOSoundsPlayed[i]) {
						availableSoundIndices.push_back(i);
					}
				}

				if (!availableSoundIndices.empty()) {
					// Random selection from available sounds
					int randomIndex = rand() % availableSoundIndices.size();
					int selectedSoundIndex = availableSoundIndices[randomIndex];
					int selectedSound = bossAttackVOsounds[selectedSoundIndex];

					// Mark this sound as played
					bossAttackVOSoundsPlayed[selectedSoundIndex] = true;

					// Play the selected sound
					audioM.PlaySFX(selectedSound);

				}
			}
		}

		if (hair->GetComponent<Animation>().GetCurrentFrame() == 8)
		{
			if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
            {
                  auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                  // Play different hit sounds based on enemy type
                           
                  std::vector<int> bossSmacksounds = {
                  audioM.GetSFXindex("BossAttack1"),
                  audioM.GetSFXindex("BossAttack2"),
                  audioM.GetSFXindex("BossAttack3")
                  };

                // Remove invalid (-1) sounds
                bossSmacksounds.erase(
                    std::remove(bossSmacksounds.begin(), bossSmacksounds.end(), -1),
                    bossSmacksounds.end()
                );

                // If no valid sounds found, try to use the original as fallback
                if (bossSmacksounds.empty()) {
                    int bossSmackOriginal = audioM.GetSFXindex("BossAttack1");
                    if (bossSmackOriginal != -1) {
                        bossSmacksounds.push_back(bossSmackOriginal);
                    }
                }

                // Keep track of which sounds have been played (static at class level)
                static std::vector<bool> bossSmackSoundsPlayed;

                // Initialize if first time or size changed
                if (bossSmackSoundsPlayed.size() != bossSmacksounds.size()) {
                    bossSmackSoundsPlayed.resize(bossSmacksounds.size(), false);
                }

                // Check if all sounds have been played
                bool allPlayed = true;
                for (bool played : bossSmackSoundsPlayed) {
                    if (!played) {
                        allPlayed = false;
                        break;
                    }
                }

                // If all sounds have been played, reset all to unplayed
                if (allPlayed) {
                    std::fill(bossSmackSoundsPlayed.begin(), bossSmackSoundsPlayed.end(), false);
                }

                // Get sounds that haven't been played yet
                std::vector<int> availableSoundIndices;
                for (int i = 0; i < bossSmacksounds.size(); i++) {
                    if (!bossSmackSoundsPlayed[i]) {
                        availableSoundIndices.push_back(i);
                    }
                }

				if (!availableSoundIndices.empty()) {
					// Random selection from available sounds
					int randomIndex = rand() % availableSoundIndices.size();
					int selectedSoundIndex = availableSoundIndices[randomIndex];
					int selectedSound = bossSmacksounds[selectedSoundIndex];

					// Mark this sound as played
					bossSmackSoundsPlayed[selectedSoundIndex] = true;

					// Play the selected sound
					audioM.PlaySFX(selectedSound);

				}
			}
			//camera shake
			ECS::GetInstance().GetSystem<Camera>()->StartShake(1.f, 10);
		}

		if (atk)
		{
			if (hairHitBox->GetActive())
			{
				HairHit(playerObj);
			}

			if (hair->GetComponent<Animation>().GetCurrentFrame() == 33)
			{
				hairHitBox->SetActive(false);

				if (atkdelay < 3.f)
				{
					atkdelay += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
				}
				else
				{
					if (hair->GetComponent<SpriteRender>().flipX == false)
					{
						if (hair->GetComponent<Transform>().position.x > hairPosX - 1000.f)
						{
							hair->GetComponent<Transform>().position.x -= 300 * g_FrameRateController.GetFixedDeltaTime();
						}
						else
						{
							atk = false;
							delay = 0.f;
							atkdelay = 0.f;
						}
					}
					else
					{
						if (hair->GetComponent<Transform>().position.x < hairPosX + 1000.f)
						{
							hair->GetComponent<Transform>().position.x += 300 * g_FrameRateController.GetFixedDeltaTime();
						}
						else
						{
							atk = false;
							delay = 0.f;
							atkdelay = 0.f;
						}
					}
				}
			}
		}

		// if (hair->GetComponent<Animation>().GetCurrentFrame() == 33)
		//{
		//	std::cout << "RESTART" << std::endl;
		//	hair->GetComponent<Animation>().RestartAnimation();
		// }
	}
	void BossManager::SpawnMonster(float x, float y)
	{
		// spawn monster
		static int number = 0;

		for (int i = 0; i < 2; i++)
		{
			GameObject* cloneObject;
			GameObject* cloneObject2;
			cloneObject2 = GameObjectManager::GetInstance().GetGOByTag("ShadowClone");

			if (numOfBlob % 2 == 0)
			{
				cloneObject = GameObjectManager::GetInstance().GetGOByTag("FishClone");
			}
			else
			{
				cloneObject = GameObjectManager::GetInstance().GetGOByTag("WormClone");
			}

			if (cloneObject != nullptr)
			{
				if (numOfBlob % 2 == 0)
				{
					std::string name = "6_EnemyFish" + std::to_string(number++);
					std::string name2 = name + "_Shadow";

					GameObject& newObject = GameObjectManager::GetInstance().CloneObject(*cloneObject, name, "Enemy");
					GameObject& shadow = GameObjectManager::GetInstance().CloneObject(*cloneObject2, name2, "EnemyShadow");

					newObject.GetComponent<Transform>().position.x = x - 100.f + i * 100.f;
					newObject.GetComponent<Transform>().position.x = std::clamp(newObject.GetComponent<Transform>().position.x, 7740.f, 9100.f);
					newObject.GetComponent<Transform>().position.y = y;

					if (newObject.HasComponent<Enemy>())
					{
						newObject.GetComponent<Enemy>() = Enemy(x, y, Enemy::FISH, newObject.GetInstanceID());
						newObject.GetComponent<SpriteRender>().layer = DYNAMIC_FRONT;
					}
					else
					{
						newObject.AddComponent(Enemy{ x, y, Enemy::FISH, newObject.GetInstanceID() });
					}

					if (shadow.HasComponent<SpriteRender>())
					{
						shadow.GetComponent<SpriteRender>().texturePath = "../Assets/Textures/Fish_Shadow.png";
						shadow.GetComponent<SpriteRender>().layer = SUB_DYNAMIC_FRONT;
					}
					else
					{
						SpriteRender sr = { "../Assets/Textures/Fish_Shadow.png", SPRITE_SHAPE::BOX, 6 };
						shadow.AddComponent<SpriteRender>(sr);
					}

					int behavior = (std::rand() % 3) + 1; // Generates 1, 2, or 3
					if (behavior == 1)
					{
						newObject.GetComponent<Enemy>().attackRange -= 50.f;
					}
					else if (behavior == 2)
					{
						newObject.GetComponent<Enemy>().attackRange -= 100.f;
					}
					else
					{
						// No change
					}

					ECS::GetInstance().GetSystem<DungeonManager>()->rooms[6].enemies.push_back(newObject.GetInstanceID());
					ECS::GetInstance().GetSystem<DungeonManager>()->rooms[6].entities.push_back(newObject.GetInstanceID());
					shadow.SetActive(true);
				}
				else
				{
					std::string name = "6_EnemyWorm" + std::to_string(number++);
					std::string name2 = name + "_Shadow";

					GameObject& newObject = GameObjectManager::GetInstance().CloneObject(*cloneObject, name, "Enemy");
					GameObject& shadow = GameObjectManager::GetInstance().CloneObject(*cloneObject2, name2, "EnemyShadow");

					newObject.GetComponent<Transform>().position.x = x - 100.f + i * 100.f;
					std::cout << newObject.GetComponent<Transform>().position.x << std::endl;
					newObject.GetComponent<Transform>().position.x = std::clamp(newObject.GetComponent<Transform>().position.x, 7740.f, 9100.f);
					newObject.GetComponent<Transform>().position.y = y;

					if (newObject.HasComponent<Enemy>())
					{
						newObject.GetComponent<Enemy>() = Enemy(x, y, Enemy::WORM, newObject.GetInstanceID());
						newObject.GetComponent<SpriteRender>().layer = DYNAMIC_FRONT;
					}
					else
					{
						newObject.AddComponent(Enemy{ x, y, Enemy::WORM, newObject.GetInstanceID() });
					}

					if (shadow.HasComponent<SpriteRender>())
					{
						shadow.GetComponent<SpriteRender>().texturePath = "../Assets/Textures/Worm_shadow.png";
						shadow.GetComponent<SpriteRender>().layer = SUB_DYNAMIC_FRONT;
					}
					else
					{
						SpriteRender sr = { "../Assets/Textures/Worm_shadow.png", SPRITE_SHAPE::BOX, 6 };
						shadow.AddComponent<SpriteRender>(sr);
					}

					int behavior = (std::rand() % 3) + 1; // Generates 1, 2, or 3
					if (behavior == 1)
					{
						newObject.GetComponent<Enemy>().attackRange -= 50.f;
					}
					else if (behavior == 2)
					{
						newObject.GetComponent<Enemy>().attackRange -= 100.f;
					}
					else
					{
						// No change
					}

					ECS::GetInstance().GetSystem<DungeonManager>()->rooms[6].enemies.push_back(newObject.GetInstanceID());
					ECS::GetInstance().GetSystem<DungeonManager>()->rooms[6].entities.push_back(newObject.GetInstanceID());
					shadow.SetActive(true);
				}

			}
		}

		ECS::GetInstance().GetSystem<EnemyManager>()->UpdateEnemyList();
	}
	void BossManager::HairHit(GameObject* player)
	{
		// Boss hitbox
		float hairLeft = hairHitBox->GetComponent<Transform>().position.x - 0.5f * hairHitBox->GetComponent<Transform>().scale.x;
		float hairRight = hairHitBox->GetComponent<Transform>().position.x + 0.4f * hairHitBox->GetComponent<Transform>().scale.x;
		float hairTop = hairHitBox->GetComponent<Transform>().position.y+100.f + 0.5f * hairHitBox->GetComponent<Transform>().scale.y;
		float hairBottom = hairHitBox->GetComponent<Transform>().position.y+100.f - 0.55f * hairHitBox->GetComponent<Transform>().scale.y;

		// Player hitbox
		float playerLeft = player->GetComponent<Transform>().position.x - 0.5f * player->GetComponent<Transform>().scale.x;
		float playerRight = player->GetComponent<Transform>().position.x + 0.5f * player->GetComponent<Transform>().scale.x;
		float playerTop = player->GetComponent<Transform>().position.y;// +0.3f * player->GetComponent<Transform>().scale.y;
		float playerBottom = player->GetComponent<Transform>().position.y - 0.25f * player->GetComponent<Transform>().scale.y;


		if (!isHairAtk && hair->GetComponent<Animation>().GetCurrentFrame() >= 9 && hair->GetComponent<Animation>().GetCurrentFrame() <= 12)
		{
			if (hair->GetComponent<SpriteRender>().flipX) //on right
			{
				if (playerRight < hairLeft && playerLeft > hairRight-200.f && playerTop < hairTop && playerBottom > hairBottom)
				{
					// Collision detected, apply damage or knockback logic
					if (!SceneManager::GetInstance().enable_cheatmode) // Dont take dmg if cheat mode is enabled
						player->GetComponent<Player>().currentHealth -= 30;

					ECS::GetInstance().GetSystem<PlayerManager>()->OnCollisionEnter(hairHitBox->GetInstanceID());

					if (player->GetComponent<Player>().currentHealth < 0)
						player->GetComponent<Player>().currentHealth = 0;

					isHairAtk = true;
				}

			}
			else
			{
				if (playerRight > hairLeft && playerLeft < hairRight && playerTop < hairTop && playerBottom > hairBottom)
				{
					// Collision detected, apply damage or knockback logic
					if (!SceneManager::GetInstance().enable_cheatmode) // Dont take dmg if cheat mode is enabled
						player->GetComponent<Player>().currentHealth -= 30;

					ECS::GetInstance().GetSystem<PlayerManager>()->OnCollisionEnter(hairHitBox->GetInstanceID());

					if (player->GetComponent<Player>().currentHealth < 0)
						player->GetComponent<Player>().currentHealth = 0;

					isHairAtk = true;

				}
			}
		}
	}

	void BossManager::BossTakeDMG()
	{
		bossCom.health -= playerObj->GetComponent<Player>().comboDamage;
		if (bossCom.health < 0)
		{
			bossCom.health = 0;
		}
	}
	void BossManager::SetBossPhase(int state)
	{
		bossCom.BossPhase = state;
		std::clamp(bossCom.BossPhase, 0, 2);
	}
	int BossManager::GetBossPhase()
	{
		return bossCom.BossPhase;
	}
}