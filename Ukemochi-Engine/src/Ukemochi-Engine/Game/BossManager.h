#pragma once
/* Start Header ************************************************************************/
/*!
\file       BossManager.h
\author     Tan Si Han, t.sihan, 2301264, t.sihan@digipen.edu
\date       Sept 20, 2025
\brief      This file contains the declaration of the BossManager class and related methods.

Copyright (C) 2025 DigiPen Institute of Technology.
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

		/*!***********************************************************************
		\brief
		   Init Boss for boss stage
		\note
		   Init Boss before the boss stage
		*************************************************************************/
		void InitBoss();

		/*!***********************************************************************
		\brief
		   Update Boss for boss stage
		\note
		   Update Boss before the boss stage
		*************************************************************************/
		void UpdateBoss();

		/*!***********************************************************************
		\brief
		   Update Boss for boss phase 1
		\note
		   Update Boss before the boss phase 1
		*************************************************************************/
		void Phase1();

		/*!***********************************************************************
		\brief
		   Update Boss for boss phase 2
		\note
		   Update Boss before the boss phase 2
		*************************************************************************/
		void Phase2();

		/*!***********************************************************************
		\brief
		   Spawn Monster when blob explode
		\param x
			position x for the monster
		\param y
			position y for the monster
		\note
			Spawn Monster when blob explode
		*************************************************************************/
		void SpawnMonster(float x, float y);

		/*!***********************************************************************
		\brief
		   Hair collision check with player
		\param player
			ptr to the player obj
		\note
			collision check between player and hair
		*************************************************************************/
		void HairHit(GameObject* player);

		/*!***********************************************************************
		\brief
		   boss take dmg from player
		\note
			collision response when player hit boss
		*************************************************************************/
		void BossTakeDMG();

		/*!***********************************************************************
		\brief
		   boss set phase
		\param state
			the state for the boss to set
		\note
			boss set phase between 0 - 2
		*************************************************************************/
		void SetBossPhase(int state);

		/*!***********************************************************************
		\brief
		   getter function
		\return int
			the state for the boss
		\note
			retrun boss state
		*************************************************************************/
		int GetBossPhase();

	private:
		GameObject* playerObj;
		GameObject* hair;
		GameObject* hairHitBox;
		GameObject* blob;
		GameObject* boss;
		Boss bossCom;
		SpriteRender hairSR;
		float blobsize;
		int numOfBlob;
		bool spawnBlob;
		float hairPosX;
		bool isHairAtk;
		bool atk;
	};
}