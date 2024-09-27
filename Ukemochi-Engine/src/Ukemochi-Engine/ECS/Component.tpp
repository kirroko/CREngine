/* Start Header
*****************************************************************/
/*!
\file	Component.tpp
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	15/09/24
\brief	To manage all the the different components that needed to be added or removed

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
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
	ComponentTypeID ComponentManager::GetComponentType()
	{
		const char* typeName = typeid(T).name();

		assert(m_ComponentTypes.find(typeName) != m_ComponentTypes.end() && "Component not registered before use.");

		return m_ComponentTypes[typeName];
	}

	template <typename T>
	void ComponentManager::AddComponent(EntityID entity, T component)
	{
		GetComponentArray<T>()->InsertData(entity, component);
	}

	template <typename T>
	void ComponentManager::RemoveComponent(EntityID entity)
	{
		GetComponentArray<T>()->RemoveData(entity);
	}

	template <typename T>
	T& ComponentManager::GetComponent(EntityID entity)
	{
		return GetComponentArray<T>()->GetData(entity);
	}
}