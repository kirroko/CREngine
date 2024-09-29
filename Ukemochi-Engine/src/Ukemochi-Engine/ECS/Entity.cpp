/* Start Header
*****************************************************************/
/*!
\file	Entity.cpp
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	15/09/24
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
	/**
	 * @brief Take an ID from the front of the queue and return to user
	 *
	 * @return EntityID a entity ID free for usage
	 */
	EntityID EntityManager::CreateEntity()
	{
		assert(m_LivingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

		EntityID id = m_AvailableEntities.front();
		m_AvailableEntities.pop();
		++m_LivingEntityCount;

		return id;
	}

	/**
	 * @brief Reset entity's corresponding component signature and push back the EntityID into the queue for recycling
	 * 
	 * @param entity The ID to release
	 */
	void EntityManager::DestroyEntity(EntityID entity)
	{
		m_Signatures[entity].reset();

		m_AvailableEntities.push(entity);
		--m_LivingEntityCount;
	}

	void EntityManager::SetSignature(EntityID entity, SignatureID signature)
	{
		m_Signatures[entity] = signature;
	}

	SignatureID EntityManager::GetSignature(EntityID entity)
	{
		return m_Signatures[entity];
	}
}