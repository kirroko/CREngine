#include "PreCompile.h"
#include "BossManager.h"
#include "../Factory/GameObjectManager.h"

namespace Ukemochi
{
	void BossManager::InitBoss()
	{

		hair = GameObjectManager::GetInstance().GetGOByTag("Hair");
		blob = GameObjectManager::GetInstance().GetGOByTag("Blob");
		hairHitBox = GameObjectManager::GetInstance().GetGOByTag("HitBox");
		playerObj = GameObjectManager::GetInstance().GetGOByTag("Player");
		boss = GameObjectManager::GetInstance().GetGOByTag("Boss")->GetComponent<Boss>();
	}
	void BossManager::UpdateBoss()
	{
		if (boss.BossPhase == 1)
		{
			if (boss.health <= 0)
			{
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
	}
	void BossManager::Phase2()
	{
	}
}