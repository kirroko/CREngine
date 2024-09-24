/* Start Header
*****************************************************************/
/*!
\file Entity.h
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
#pragma once

#include "PreCompile.h"

namespace Ukemochi
{
	using EntityID = unsigned long int;       // A type alias representing Entity type (Their ID)
	using ComponentTypeID = unsigned char; // A type alias representing Component type (Their ID)
	const EntityID MAX_ENTITIES = 5000;
	const ComponentTypeID MAX_COMPONENTS = 32;
	using SignatureID = std::bitset<MAX_COMPONENTS>; // a type alias representing components bit signature (0x111 to represent 3 components)

	// Manage the Entities
	class EntityManager
	{
		std::queue<EntityID> m_AvailableEntities{};           // Queue of unused entity IDs
		std::array<SignatureID, MAX_ENTITIES> m_Signatures{}; // Array of signatures where the index corresponds to the entity ID

		unsigned long int m_ulLivingEntityCount{};

	public:
		EntityManager()
		{
			for (std::size_t entity = 0; entity < MAX_ENTITIES; ++entity)
			{
				m_AvailableEntities.push(entity);
			}
		};

		EntityID CreateEntity();
		void DestroyEntity(EntityID entity);
		void SetSignature(EntityID entity, SignatureID signature);
		SignatureID GetSignature(EntityID entity);
	};
}
// 0x4B45414E
