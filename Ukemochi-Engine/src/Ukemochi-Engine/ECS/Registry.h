/* Start Header
*****************************************************************/
/*!
\file	Registry.h
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	15/09/24
\brief A registry where all components are stored.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once

#include <unordered_map>
#include "Entity.h"

namespace Ukemochi
{
	class IComponentArray
	{
	public:
		virtual ~IComponentArray() = default;
		virtual void EntityDestroyed(EntityID entity) = 0;
	};

	// The component array class that stores the components of type T in a contiguous array.
	template <typename T>
	class ComponentArray : public IComponentArray
	{
	public:
		void InsertData(EntityID entity, T component);

		void RemoveData(EntityID entity);

		T& GetData(EntityID entity);

		void EntityDestroyed(EntityID entity) override;

		// Method to check if an entity has a component
		// This function helps verify entity existence before accessing its components, preventing potential errors.
		bool HasEntity(EntityID entity) const {
			return m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end();
		}

	private:
		// The packed array of components (of generic type T),
		// set to a specified maximum amount, matching the maximum number
		// of entities allowed to exist simultaneously, so that each entity
		// has a unique spot.
		std::array<T, MAX_ENTITIES> m_ComponentArray;

		// Map from an entity ID to an array index.
		std::unordered_map<EntityID, size_t> m_EntityToIndexMap;

		// Map from an array index to an entity ID.
		std::unordered_map<size_t, EntityID> m_IndexToEntityMap;

		// Total size of valid entries in the array.
		size_t m_Size;
	};
}
#include "Registry.tpp"
// 0x4B45414E
