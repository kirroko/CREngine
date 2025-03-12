#include "PreCompile.h"
#include "BossManager.h"
#include "../Factory/GameObjectManager.h"
#include "DungeonManager.h"
#include "../FrameController.h"
#include "../Game/EnemyManager.h"

namespace Ukemochi
{
	void BossManager::InitBoss()
	{

		hair = GameObjectManager::GetInstance().GetGOByTag("Hair");
		hair->GetComponent<Animation>().SetAnimation("HairAtk");
		hairHitBox = GameObjectManager::GetInstance().GetGOByTag("HitBox");
		hairPosX = hair->GetComponent<Transform>().position.x;
		hair->SetActive(false);
		hairHitBox->SetActive(false);

		blob = GameObjectManager::GetInstance().GetGOByTag("BlobClone");
		blob->GetComponent<Animation>().SetAnimation("Idle");
		blobsize = blob->GetComponent<Transform>().scale.x;
		blob->GetComponent<Transform>().scale = Vec2();
		spawnBlob = false;
		numOfBlob = 0;

		playerObj = GameObjectManager::GetInstance().GetGOByTag("Player");
		boss = GameObjectManager::GetInstance().GetGOByTag("Boss");
		bossCom = GameObjectManager::GetInstance().GetGOByTag("Boss")->GetComponent<Boss>();
	}
	void BossManager::UpdateBoss()
	{
		if (ECS::GetInstance().GetSystem<DungeonManager>()->current_room_id != 6)
			return;

		if (bossCom.BossPhase == 1)
		{
			//CHANGE TO IF THERE NO ENEMY LEFT AND NUM OF BLOB MORE THAN 2
			if (!ECS::GetInstance().GetSystem<DungeonManager>()->enemy_alive && numOfBlob >= 4)
			{
				//play animation
				GameObjectManager::GetInstance().GetGOByTag("Boss")->GetComponent<Animation>().SetAnimationUninterrupted("Change");
				//animation finish then proceed
				bossCom.BossPhase = 2;
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
				//WIN
			}
			else
			{
				Phase2();
			}
		}
	}
	void BossManager::Phase1()
	{
		GameObjectManager::GetInstance().GetGOByTag("Boss")->GetComponent<Animation>().SetAnimation("Idle1");
		//attack
		//timer end play attack
		if (bossCom.waitTime < 7.f)
		{
			bossCom.waitTime += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
		}
		else
		{
			if (numOfBlob <= 3)
			{
				spawnBlob = true;
				//play animation
				GameObjectManager::GetInstance().GetGOByTag("Boss")->GetComponent<Animation>().SetAnimationUninterrupted("Change");
				numOfBlob++;
			}
		}
		//spawn blob blob
		//scale from 0 - orignal size
		if (spawnBlob && numOfBlob <= 3)
		{
			//spawn blob blob
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
		GameObjectManager::GetInstance().GetGOByTag("Boss")->GetComponent<Animation>().SetAnimation("Idle2");
		static float delay = 0.f;
		static float atkdelay = 0.f;
		static bool atk = false;
		//hair attack
		if (delay < 7.f)
		{
			delay += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
		}
		else
		{
			if (!atk)
			{
				//set hair attack
				if (playerObj->GetComponent<Transform>().position.x > boss->GetComponent<Transform>().position.x)
				{
					//right
					hair->GetComponent<SpriteRender>().flipX = true;
					hairHitBox->GetComponent<Transform>().scale.x = -853.000f;

					hair->GetComponent<Transform>().position.x = 9375;
					hairHitBox->GetComponent<Transform>().position.x = 9200;
					hairPosX = hair->GetComponent<Transform>().position.x;
				}
				else
				{
					//left
					hair->GetComponent<SpriteRender>().flipX = false;
					hairHitBox->GetComponent<Transform>().position.x = 8100;

					hair->GetComponent<Transform>().position.x = 7955;
					hairHitBox->GetComponent<Transform>().scale.x = 853.000f;
					hairPosX = hair->GetComponent<Transform>().position.x;
				}
				hair->SetActive(true);
				hair->GetComponent<Animation>().RestartAnimation();

				hair->GetComponent<Transform>().position.y = playerObj->GetComponent<Rigidbody2D>().position.y;
				hairHitBox->GetComponent<Transform>().position.y = hair->GetComponent<Transform>().position.y-50.f;
				hairHitBox->SetActive(true);
				atk = true;

			}
		}

		if (atk)
		{
			std::cout << hair->GetComponent<Animation>().GetCurrentFrame() << std::endl;
			if (hair->GetComponent<Animation>().GetCurrentFrame() == 33)
			{
				hairHitBox->SetActive(false);

				if (atkdelay < 4.f)
				{
					atkdelay += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
				}
				else
				{
					if (hair->GetComponent<SpriteRender>().flipX == false)
					{
						if (hair->GetComponent<Transform>().position.x > hairPosX - 1000.f)
						{
							hair->GetComponent<Transform>().position.x -= 1;
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
							hair->GetComponent<Transform>().position.x += 1;
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

		//if (hair->GetComponent<Animation>().GetCurrentFrame() == 33)
		//{
		//	std::cout << "RESTART" << std::endl;
		//	hair->GetComponent<Animation>().RestartAnimation();
		//}




	}
	void BossManager::SpawnMonster(float x, float y)
	{
		//spawn monster
		static int number = 0;

		for (int i = 0; i < 3; i++)
		{
			GameObject* cloneObject;
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
				std::string name = "6_Enemy" + std::to_string(number++);
				std::string name2 = name + "_Shadow";

				GameObject& newObject = GameObjectManager::GetInstance().CloneObject(*cloneObject, name, "Enemy");
				GameObject& shadow = GameObjectManager::GetInstance().CloneObject(*cloneObject, name2, "EnemyShadow");

				newObject.GetComponent<Transform>().position.x = x;
				newObject.GetComponent<Transform>().position.y = y;

				if (numOfBlob % 2 == 0)
				{
					if (newObject.HasComponent<Enemy>())
					{
						newObject.GetComponent<Enemy>() = Enemy(x, y, Enemy::FISH, newObject.GetInstanceID());
					}
					else
					{
						newObject.AddComponent(Enemy{ x, y, Enemy::FISH, newObject.GetInstanceID() });
					}

					if (shadow.HasComponent<SpriteRender>())
					{
						shadow.GetComponent<SpriteRender>().texturePath = "../Assets/Textures/Fish_Shadow.png";
						shadow.GetComponent<SpriteRender>().layer = 1;
					}
					else
					{
						SpriteRender sr = { "../Assets/Textures/Fish_Shadow.png", SPRITE_SHAPE::BOX, 1 };
						shadow.AddComponent<SpriteRender>(sr);
					}
				}
				else
				{
					if (newObject.HasComponent<Enemy>())
					{
						newObject.GetComponent<Enemy>() = Enemy(x, y, Enemy::WORM, newObject.GetInstanceID());
					}
					else
					{
						newObject.AddComponent(Enemy{ x, y, Enemy::WORM, newObject.GetInstanceID() });
					}

					if (shadow.HasComponent<SpriteRender>())
					{
						shadow.GetComponent<SpriteRender>().texturePath = "../Assets/Textures/Worm_shadow.png";
						shadow.GetComponent<SpriteRender>().layer = 1;
					}
					else
					{
						SpriteRender sr = { "../Assets/Textures/Worm_shadow.png", SPRITE_SHAPE::BOX, 1 };
						shadow.AddComponent<SpriteRender>(sr);
					}
				}
				ECS::GetInstance().GetSystem<DungeonManager>()->rooms[6].enemies.push_back(newObject.GetInstanceID());
				ECS::GetInstance().GetSystem<DungeonManager>()->rooms[6].entities.push_back(newObject.GetInstanceID());
			}
		
		}

		ECS::GetInstance().GetSystem<EnemyManager>()->UpdateEnemyList();
	}
}