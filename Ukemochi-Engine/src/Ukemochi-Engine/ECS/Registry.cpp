/* Start Header
*****************************************************************/
/*!
\file Registry.cpp
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par junyukean.wong\@digipen.edu
\date 15/09/24
\brief This file contains the definition of the memeber functions ComponentArray

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "PreCompile.h"
#include "Registry.h"

namespace Ukemochi
{
	template <typename T>
	void ComponentArray<T>::InsertData(Entity entity, T component)
	{
		assert(m_EntityToIndexMap.find(entity) == m_EntityToIndexMap.end() && "Component added to same entity more than once.");

		// Put new entry at end and update the maps
		size_t newIndex = mSize;
		m_EntityToIndexMap[entity] = newIndex;
		m_IndexToEntityMap[newIndex] = entity;
		m_ComponentArray[newIndex] = component;
		++mSize;
	}

	template <typename T>
	void ComponentArray<T>::RemoveData(Entity entity)
	{
		assert(m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end() && "Removing non-existent component.");

		size_t indexOfRemovedEntity = m_EntityToIndexMap[entity]; // Copy element at end into deleted element's place to maintain density
		size_t indexOfLastElement = m_Size - 1;
		m_ComponentArray[indexOfRemovedEntity] = m_ComponentArray[indexOfLastElement];

		Entity entityOfLastElement = m_IndexToEntityMap[indexOfLastElement]; // Update map to point to moved spot
		m_EntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
		m_IndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

		m_EntityToIndexMap.erase(entity);
		m_IndexToEntityMap.erase(indexOfLastElement);

		--m_Size;
	}

	template <typename T>
	T& ComponentArray<T>::GetData(Entity entity)
	{
		assert(m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end() && "Retrieving non-existent component.");

		return m_ComponentArray[m_EntityToIndexMap[entity]]; // Return a reference to the entity's component
	}

	template <typename T>
	void ComponentArray<T>::EntityDestroyed(Entity entity)
	{
		if (m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end())
			RemoveData(entity); // Remove the entity's component if it existed
	}
}