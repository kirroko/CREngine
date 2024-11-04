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
	EntityID EntityManager::CreateEntity()
	{
		EntityID id = m_AvailableEntities.front();
		m_AvailableEntities.pop();
		++m_ulLivingEntityCount;

		return id;
	}

	void EntityManager::DestroyEntity(EntityID entity)
	{
		m_Signatures[entity].reset();

		m_AvailableEntities.push(entity);
		--m_ulLivingEntityCount;
	}

	void EntityManager::SetSignature(EntityID entity, SignatureID signature)
	{
		m_Signatures[entity] = signature;
	}

	SignatureID EntityManager::GetSignature(EntityID entity)
	{
		return m_Signatures[entity];
	}

	unsigned long int EntityManager::GetLivingEntityCount() const
	{
		return m_ulLivingEntityCount;
	}

	// Used to retrive a list of all active entities within the ECS
	// active is defined by having a non-empty signature
	// 0x4B45414E - No no no, This is not the jedi way
	// std::vector<EntityID> EntityManager::GetAllEntities() {
	// 	std::vector<EntityID> activeEntities;
	// 	for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
	// 		// Check if the entity has an active signature
	// 		if (m_Signatures[entity].any()) { // Adjust this condition as needed for "active" state
	// 			activeEntities.push_back(entity);
	// 		}
	// 	}
	// 	return activeEntities;
	// }
}