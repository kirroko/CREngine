/* Start Header
*****************************************************************/
/*!
\file	Entity.h
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	15/09/24
\brief	The ECS that in charge of distributing entity IDs and keeping record of which IDs are in use and which are not.

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
	using EntityID = unsigned long long int;       // A type alias representing Entity type (Their ID)
	using ComponentTypeID = unsigned char; // A type alias representing Component type (Their ID)
	const EntityID MAX_ENTITIES = 2501;   // Maximum number of entities that can be registered
	const ComponentTypeID MAX_COMPONENTS = 32; // Maximum number of components that can be registered
	using SignatureID = std::bitset<MAX_COMPONENTS>; // a type alias representing components bit signature (0x111 to represent 3 components)

	/**
	 * @brief To distribute entity IDs and keeping record of which IDs are in use and which are not. It also function to keep track what
	 */
	class EntityManager
	{
		std::queue<EntityID> m_AvailableEntities{};           // Queue of unused entity IDs
		std::array<SignatureID, MAX_ENTITIES> m_Signatures{}; // Array of signatures where the index corresponds to the entity ID

		unsigned long int m_LivingEntityCount{};

	public:
		EntityManager()
		{
			for (std::size_t entity = 0; entity < MAX_ENTITIES; ++entity)
			{
				m_AvailableEntities.push(entity);
			}
		};

		/**
		 * @brief Take an ID from the front of the queue and return to user
		 *
		 * @return EntityID a entity ID free for usage
		 */
		EntityID CreateEntity();
		/**
		 * @brief Reset entity's corresponding component signature and push back the EntityID into the queue for recycling
		 *
		 * @param entity The ID to release
		 */
		void DestroyEntity(EntityID entity);
		void SetSignature(EntityID entity, SignatureID signature);
		SignatureID GetSignature(EntityID entity);
	};
}
// 0x4B45414E
