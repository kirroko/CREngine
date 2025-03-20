#pragma once
/* Start Header ************************************************************************/
/*!
\file       BossManager.h
\author     Tan Si Han, t.sihan, 2301264, t.sihan@digipen.edu
\date       Sept 20, 2024
\brief      This file contains the declaration of the BossManager class and related methods.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
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
		GameObject* boss;
		Boss bossCom;
		float blobsize;
		int numOfBlob;
		bool spawnBlob;
		float hairPosX;
	};
}