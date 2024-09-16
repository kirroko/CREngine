/* Start Header
*****************************************************************/
/*!
\file Entity.cpp
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par junyukean.wong\@digipen.edu
\date 15/09/24
\brief The ECS that in charge of distributing entity IDs and keeping record of which IDs are in use and which are not.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "PreCompile.h"
#include "Entity.h"

namespace Ukemochi
{
	Entity EntityManager::CreateEntity()
	{
		Entity id = m_AvailableEntities.front();
		m_AvailableEntities.pop();
		++m_ulLivingEntityCount;

		return id;
	}

	void EntityManager::DestroyEntity(Entity entity)
	{
		m_Signatures[entity].reset();

		m_AvailableEntities.push(entity);
		--m_ulLivingEntityCount;
	}

	void EntityManager::SetSignature(Entity entity, Signature signature)
	{
		m_Signatures[entity] = signature;
	}

	Signature EntityManager::GetSignature(Entity entity)
	{
		return m_Signatures[entity];
	}
}