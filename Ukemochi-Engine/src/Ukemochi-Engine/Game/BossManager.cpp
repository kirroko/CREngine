/* Start Header ************************************************************************/
/*!
\file       BossManager.cpp
\author     Tan Si Han, t.sihan, 2301264, t.sihan@digipen.edu (90%)
\co-authors HURNG Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu (10%)
\date       Feb 05, 2024
\brief      This file contains the definition of the BossManager class and related methods.

Copyright (C) 2024 DigiPen Institute of Technology.
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

namespace Ukemochi
{
	void BossManager::InitBoss()
	{

		hair = GameObjectManager::GetInstance().GetGOByTag("Hair");
		if (hair && hair->HasComponent<Animation>())
		{
			hair->GetComponent<Animation>().SetAnimation("HairAtk");
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
		}

		isHairAtk = true;
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
			if (bossCom.BossPhase == 1)
			{
				hair->SetActive(false);
				hairHitBox->SetActive(false);
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
			else
			{
				if (bossCom.health <= 0)
				{
					// WIN
					boss->SetActive(false);
					hair->SetActive(false);
				}
				else
				{
					Phase2();
				}
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
			GameObject *cloneObject = GameObjectManager::GetInstance().GetGOByTag("BlobClone");
			if (cloneObject != nullptr)
			{
				std::string name = "blob" + std::to_string(number++);
				GameObject &newObject = GameObjectManager::GetInstance().CloneObject(*cloneObject, name, "Blob");

				newObject.GetComponent<Transform>().position.x = playerObj->GetComponent<Transform>().position.x;
				newObject.GetComponent<Transform>().position.y = playerObj->GetComponent<Transform>().position.y;
			}
			spawnBlob = false;
			bossCom.waitTime = 0;
		}
	}
	void BossManager::Phase2()
	{
		GameObjectManager::GetInstance().GetGOByTag("Boss")->GetComponent<Animation>().SetAnimation("Idle2");
		static float delay = 0.f;
		static float atkdelay = 0.f;
		static bool atk = false;
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

					hair->GetComponent<Transform>().position.y = playerObj->GetComponent<Rigidbody2D>().position.y-25.f;
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

				atk = true;
				isHairAtk = false;
			}
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

		for (int i = 0; i < 3; i++)
		{
			GameObject *cloneObject;
			GameObject* cloneObject2;
			cloneObject2 = GameObjectManager::GetInstance().GetGOByTag("ShadowClone");

			if (numOfBlob % 2 == 0)
			{
				cloneObject = GameObjectManager::GetInstance().GetGOByTag("Enemy");
			}
			else
			{
				cloneObject = GameObjectManager::GetInstance().GetGOByTag("EnemyClone");
			}

			if (cloneObject != nullptr)
			{
				if (numOfBlob % 2 == 0)
				{
					std::string name = "6_EnemyFish" + std::to_string(number++);
					std::string name2 = name + "_Shadow";

					GameObject& newObject = GameObjectManager::GetInstance().CloneObject(*cloneObject, name, "Enemy");
					GameObject& shadow = GameObjectManager::GetInstance().CloneObject(*cloneObject2, name2, "EnemyShadow");

					newObject.GetComponent<Transform>().position.x = x;
					newObject.GetComponent<Transform>().position.y = y;

					if (newObject.HasComponent<Enemy>())
					{
						newObject.GetComponent<Enemy>() = Enemy(x, y, Enemy::FISH, newObject.GetInstanceID());
						newObject.GetComponent<SpriteRender>().layer = 6;
					}
					else
					{
						newObject.AddComponent(Enemy{x, y, Enemy::FISH, newObject.GetInstanceID()});
					}

					if (shadow.HasComponent<SpriteRender>())
					{
						shadow.GetComponent<SpriteRender>().texturePath = "../Assets/Textures/Fish_Shadow.png";
						shadow.GetComponent<SpriteRender>().layer = 6;
					}
					else
					{
						SpriteRender sr = {"../Assets/Textures/Fish_Shadow.png", SPRITE_SHAPE::BOX, 6};
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

					newObject.GetComponent<Transform>().position.x = x;
					newObject.GetComponent<Transform>().position.y = y;

					if (newObject.HasComponent<Enemy>())
					{
						newObject.GetComponent<Enemy>() = Enemy(x, y, Enemy::WORM, newObject.GetInstanceID());
						newObject.GetComponent<SpriteRender>().layer = 6;
					}
					else
					{
						newObject.AddComponent(Enemy{x, y, Enemy::WORM, newObject.GetInstanceID()});
					}

					if (shadow.HasComponent<SpriteRender>())
					{
						shadow.GetComponent<SpriteRender>().texturePath = "../Assets/Textures/Worm_shadow.png";
						shadow.GetComponent<SpriteRender>().layer = 6;
					}
					else
					{
						SpriteRender sr = {"../Assets/Textures/Worm_shadow.png", SPRITE_SHAPE::BOX, 6};
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
		float hairLeft = hairHitBox->GetComponent<Transform>().position.x - 0.5f*hairHitBox->GetComponent<Transform>().scale.x;
		float hairRight = hairHitBox->GetComponent<Transform>().position.x + 0.5f * hairHitBox->GetComponent<Transform>().scale.x;
		float hairTop = hairHitBox->GetComponent<Transform>().position.y - 0.5f * hairHitBox->GetComponent<Transform>().scale.y;
		float hairBottom = hairHitBox->GetComponent<Transform>().position.y + 0.5f * hairHitBox->GetComponent<Transform>().scale.y;

		// Player hitbox
		float playerLeft = player->GetComponent<Transform>().position.x - 0.5f * player->GetComponent<Transform>().scale.x;
		float playerRight = player->GetComponent<Transform>().position.x + 0.5f * player->GetComponent<Transform>().scale.x;
		float playerTop = player->GetComponent<Transform>().position.y - 0.3f * player->GetComponent<Transform>().scale.y;
		float playerBottom = player->GetComponent<Transform>().position.y + 0.3f * player->GetComponent<Transform>().scale.y;

		// AABB Collision check
		if (playerRight > hairLeft && playerLeft < hairRight &&
			playerBottom > hairTop && playerTop < hairBottom && !isHairAtk && hair->GetComponent<Animation>().GetCurrentFrame() >= 10) {

			// Collision detected, apply damage or knockback logic
			//player->GetComponent<Player>().currentHealth -= 50;

			ECS::GetInstance().GetSystem<PlayerManager>()->OnCollisionEnter(hairHitBox->GetInstanceID());

			if (player->GetComponent<Player>().currentHealth < 0)
				player->GetComponent<Player>().currentHealth = 0;
			isHairAtk = true;
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
}