/* Start Header ************************************************************************/
/*!
\file       ECS.h
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date       Sep 15, 2024
\brief      The ECS will coordinator between the Component, Entity and System managers.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include "Component.h" // ComponentManager
#include "Entity.h"    // EntityManager
#include "Systems.h"   // SystemManager
#include "Components.h" // DO NOT REMOVE

namespace Ukemochi
{
	class ECS
	{
		std::unique_ptr<ComponentManager> m_ComponentManager;
		std::unique_ptr<EntityManager> m_EntityManager;
		std::unique_ptr<SystemManager> m_SystemManager;

		ECS() = default;
		~ECS() = default;
	public:
		static ECS& GetInstance()
		{
			static ECS instance;
			return instance;
		}
		
		ECS(const ECS&) = delete;
		ECS& operator=(const ECS&) = delete;
		ECS(ECS&&) = delete;
		ECS& operator=(ECS&&) = delete;
		
		// Create pointers to each manager
		void Init();

		EntityID CreateEntity();

		void ReloadEntityManager();

		void DestroyEntity(EntityID entity);

		EntityID CloneEntity(EntityID entity);

		/**
		 * @brief Get the number of living entities in the ECS
		 * @return The number of living entities
		 */
		unsigned long int GetLivingEntityCount() const;

		/**
		 * @brief Register a component type with the ECS
		 * @tparam T The component type to register
		 */
		template<typename T>
		void RegisterComponent();

		/**
		 * @brief Add a component to an entity.
		 * @tparam T The component type to add
		 * @param entity The entity to add the component to
		 * @param component The component to add
		 */
		template<typename T>
		void AddComponent(EntityID entity, T component);

		template<typename T>
		void RemoveComponent(EntityID entity);

		template<typename T>
		T& GetComponent(EntityID entity);

		template<typename T>
		ComponentTypeID GetComponentType();

		// System methods
		template<typename T>
		std::shared_ptr<T> RegisterSystem();

		template<typename T>
		std::shared_ptr<T> GetSystem();

		template<typename T>
		void SetSystemSignature(SignatureID signature);

		// Manage and verify the relationship between entity and their components within ECS
		template<typename T>
		bool HasComponent(EntityID entity) const
		{
			return m_ComponentManager->HasComponent<T>(entity);
		}
	};
}
#include "ECS.tpp"
// 0x4B45414E
