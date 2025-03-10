#include "PreCompile.h"
#include "BossManager.h"
#include "../Factory/GameObjectManager.h"
#include "DungeonManager.h"
#include "../FrameController.h"

namespace Ukemochi
{
	void BossManager::InitBoss()
	{

		hair = GameObjectManager::GetInstance().GetGOByTag("Hair");
		hair->GetComponent<Animation>().SetAnimation("HairAtk");
		hairHitBox = GameObjectManager::GetInstance().GetGOByTag("HitBox");

		blob = GameObjectManager::GetInstance().GetGOByTag("BlobClone");
		blob->GetComponent<Animation>().SetAnimation("Idle");
		blobsize = blob->GetComponent<Transform>().scale.x;
		blob->GetComponent<Transform>().scale = Vec2();
		spawnBlob = false;

		playerObj = GameObjectManager::GetInstance().GetGOByTag("Player");

		boss = GameObjectManager::GetInstance().GetGOByTag("Boss")->GetComponent<Boss>();
	}
	void BossManager::UpdateBoss()
	{
		if (ECS::GetInstance().GetSystem<DungeonManager>()->current_room_id != 6)
			return;

		if (boss.BossPhase == 1)
		{
			if (boss.health <= 0)
			{
				//play animation
				GameObjectManager::GetInstance().GetGOByTag("Boss")->GetComponent<Animation>().SetAnimation("Change");
				//animation finish then proceed
				boss.BossPhase = 2;
			}
			else
			{
				Phase1();
			}
		}
		else
		{
			if (boss.health <= 0)
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
		if (boss.waitTime < 2.0f)
		{
			boss.waitTime += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
		}
		else
		{
			spawnBlob = true;
			boss.waitTime = 0;
		}
		//spawn blob blob
		//scale from 0 - orignal size
		if (spawnBlob)
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
		}

		for (auto& entity : m_Entities)
		{
			std::string tag = GameObjectManager::GetInstance().GetGO(entity)->GetTag();

			// Skip if the entity is not active
			if (!GameObjectManager::GetInstance().GetGO(entity)->GetActive())
				continue;

			if (tag == "Blob")
			{
				if (GameObjectManager::GetInstance().GetGO(entity)->GetComponent<Transform>().scale.x < blobsize)
				{
					GameObjectManager::GetInstance().GetGO(entity)->GetComponent<Transform>().scale.x += static_cast<float>(g_FrameRateController.GetFixedDeltaTime()) * 100.f;
					GameObjectManager::GetInstance().GetGO(entity)->GetComponent<Transform>().scale.y += static_cast<float>(g_FrameRateController.GetFixedDeltaTime()) * 100.f;
				}
				else
				{
					blob->GetComponent<Animation>().SetAnimation("Explode");

					if (blob->GetComponent<Animation>().GetCurrentFrame() == 19)
					{
						GameObjectManager::GetInstance().GetGO(entity)->SetActive(false);
						GameObjectManager::GetInstance().DestroyObject(entity);

						//spawn monster

						break;
					}
				}
			}
		}

	}
	void BossManager::Phase2()
	{
		//hair attack
		if (hair->GetComponent<Animation>().GetCurrentFrame() == 33)
		{
			std::cout << "RESTART" << std::endl;
			hair->GetComponent<Animation>().RestartAnimation();
		}
	}
}