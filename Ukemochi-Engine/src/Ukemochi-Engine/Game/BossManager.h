#pragma once
#include "../ECS/ECS.h"
#include "../Factory/GameObject.h"

namespace Ukemochi
{
	class BossManager : public System
	{
	public:
		void InitBoss();

		void UpdateBoss();

		void Phase1();

		void Phase2();

		void SpawnMonster(float x, float y);

	private:
		GameObject* playerObj;
		GameObject* hair;
		GameObject* hairHitBox;
		GameObject* blob;
		Boss boss;
		float blobsize;
		int numOfBlob;
		bool spawnBlob;
		float hairPosX;
	};
}