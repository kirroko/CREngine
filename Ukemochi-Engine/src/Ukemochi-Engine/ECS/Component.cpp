/* Start Header
*****************************************************************/
/*!
\file Component.cpp
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par junyukean.wong\@digipen.edu
\date 15/09/24
\brief This file contains the definition of the member function of ComponentManager

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "PreCompile.h"
#include "Component.h"

namespace Ukemochi
{
	template <typename T>
	std::shared_ptr<ComponentArray<T>> ComponentManager::GetComponentArray()
	{
		const char* typeName = typeid(T).name();

		return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[typeName]);
	}

	template <typename T>
	void ComponentManager::RegisterComponent()
	{
		const char* typeName = typeid(T).name();

		assert(m_ComponentTypes.find(typeName) == m_ComponentTypes.end() && "Registering component type more than once.");

		m_ComponentTypes.insert({ typeName, m_NextComponentType++ });

		m_ComponentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });
	}

	template <typename T>
	ComponentType ComponentManager::GetComponentType()
	{
		const char* typeName = typeid(T).name();

		assert(m_ComponentTypes.find(typeName) != m_ComponentTypes.end() && "Component not registered before use.");

		return m_ComponentTypes[typeName];
	}

	template <typename T>
	void ComponentManager::AddComponent(Entity entity, T component)
	{
		GetComponentArray<T>()->InsertData(entity, component);
	}

	template <typename T>
	void ComponentManager::RemoveComponent(Entity entity)
	{
		GetComponentArray<T>()->RemoveData(entity);
	}

	template <typename T>
	T& ComponentManager::GetComponent(Entity entity)
	{
		return GetComponentArray<T>()->GetData(entity);
	}

	void ComponentManager::EntityDestroyed(Entity entity)
	{
		for (auto const& pair : m_ComponentArrays) // Iterate over all component arrays
		{
			auto const& component = pair.second;

			component->EntityDestroyed(entity);
		}
	}
}