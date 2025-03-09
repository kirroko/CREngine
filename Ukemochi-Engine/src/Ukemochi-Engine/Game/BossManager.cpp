#include "PreCompile.h"
#include "BossManager.h"
#include "../Factory/GameObjectManager.h"
#include "DungeonManager.h"

namespace Ukemochi
{
	void BossManager::InitBoss()
	{

		hair = GameObjectManager::GetInstance().GetGOByTag("Hair");
		hairHitBox = GameObjectManager::GetInstance().GetGOByTag("HitBox");
		blob = GameObjectManager::GetInstance().GetGOByTag("Blob");
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
				boss.BossPhase = 2;
			}
			else
			{
				if (!hair->GetComponent<Animation>().is_playing)
				{
					std::cout << "RESTART" << std::endl;
					hair->GetComponent<Animation>().RestartAnimation();
				}
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

	}
	void BossManager::Phase2()
	{
	}
}