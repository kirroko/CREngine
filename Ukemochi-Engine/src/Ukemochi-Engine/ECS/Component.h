/* Start Header
*****************************************************************/
/*!
\file Component.h
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par junyukean.wong\@digipen.edu
\date 15/09/24
\brief To manage all the the different components that needed to be added or removed

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once

#include "Registry.h"

namespace Ukemochi
{
	// The component manager class that manages all the different component arrays that are attached to the component type
	class ComponentManager
	{
		// Here is where we store all the known component types whose ID = ComponentType (i.e transform, sprite, etc)
		std::unordered_map<const char*, ComponentTypeID> m_ComponentTypes{};

		// This is where we store all the component arrays () that is attached to ComponentType's name
		std::unordered_map<const char*, std::shared_ptr<IComponentArray>> m_ComponentArrays{}; // Map from type string pointer to a component array

		ComponentTypeID m_NextComponentType{}; // The component type to be assigned to the next registered component - starting a 0

		// Convenience function to get the statically casted pointer to the ComponentArray of type T.
		template <typename T>
		std::shared_ptr<ComponentArray<T>> GetComponentArray();

	public:
		// Register a component
		template <typename T>
		void RegisterComponent();

		template <typename T>
		ComponentTypeID GetComponentType();

		// Add a component to an entity
		template<typename T>
		void AddComponent(EntityID entity, T component);

		// Remove a component from an entity
		template<typename T>
		void RemoveComponent(EntityID entity);

		// Get a reference to a component of type T for an entity
		template<typename T>
		T& GetComponent(EntityID entity);

		// Notify all component arrays that an entity has been destroyed
		void EntityDestroyed(EntityID entity);
	};
}