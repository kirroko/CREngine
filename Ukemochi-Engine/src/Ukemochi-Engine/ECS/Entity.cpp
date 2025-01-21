/* Start Header ************************************************************************/
/*!
\file       Entity.cpp
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date       Sep 15, 2024
\brief      The ECS that in charge of distributing entity IDs and keeping record of which IDs are in use and which are not.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "Entity.h"

namespace Ukemochi
{
	/**
	 * @brief Create an entity
	 * @return The ID of the created entity
	 */
	EntityID EntityManager::CreateEntity()
	{
		EntityID id = m_AvailableEntities.front();
		m_AvailableEntities.pop();
		++m_ulLivingEntityCount;

		return id;
	}

	/**
	 * @brief Destroy an entity
	 * @param entity The entity to destroy
	 */
	void EntityManager::DestroyEntity(EntityID entity)
	{
		m_Signatures[entity].reset();

		m_AvailableEntities.push(entity);
		--m_ulLivingEntityCount;
	}

	/**
	 * @brief Set the signature of an entity
	 * @param entity The entity to set the signature of
	 * @param signature The signature to set
	 */
	void EntityManager::SetSignature(EntityID entity, SignatureID signature)
	{
		m_Signatures[entity] = signature;
	}

	/**
	 * @brief Get the signature of an entity
	 * @param entity The entity to get the signature of
	 * @return The signature of the entity
	 */
	SignatureID EntityManager::GetSignature(EntityID entity) const
	{
		return m_Signatures[entity];
	}

	/**
	 * @brief Get the number of living entities
	 * @return The number of living entities
	 */
	unsigned long int EntityManager::GetLivingEntityCount() const
	{
		return m_ulLivingEntityCount;
	}
}