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
		
		/**
		 * @brief Initialize the ECS
		 */
		void Init();

		/**
		 * @brief Create an entity
		 * @return The ID of the created entity
		 */
		EntityID CreateEntity() const;

		/**
		 * @brief Reload the entity manager
		 */
		void ReloadEntityManager();

		/**
		 * @brief Destroy an entity
		 * @param entity The entity to destroy
		 */
		void DestroyEntity(EntityID entity) const;

		/**
		 * @brief Clone an entity
		 * @param entity The entity to clone
		 * @return The ID of the cloned entity
		 */
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

		/**
		 * @brief Remove a component from an entity.
		 * @tparam T The component type to remove
		 * @param entity The entity to remove the component from
		 */
		template<typename T>
		void RemoveComponent(EntityID entity) const;

		/**
		 * @brief Get a reference to a component of type T for an entity.
		 * @tparam T The component type to get
		 * @param entity The entity to get the component from
		 * @return A reference to the component
		 */
		template<typename T>
		T& GetComponent(EntityID entity) const;

		/**
		 * @brief Get a component type ID
		 * @tparam T The component type to get the ID of
		 * @return The component type ID
		 */
		template<typename T>
		ComponentTypeID GetComponentType() const;

		/**
		 * @brief Register a system with the ECS
		 * @tparam T The system type to register
		 * @return A shared pointer to the registered system
		 */
		template<typename T>
		std::shared_ptr<T> RegisterSystem() const;

		/**
		 * @brief Get a system from the ECS
		 * @tparam T The system type to get
		 * @return A shared pointer to the system
		 */
		template<typename T>
		std::shared_ptr<T> GetSystem() const;

		/**
		 * @brief Set the signature of a system
		 * @tparam T The system type to set the signature of
		 * @param signature The signature to set
		 */
		template<typename T>
		void SetSystemSignature(SignatureID signature) const;
		
		/**
		 * @brief Check if an entity has a component
		 * Manage and verify the relationship between entity and their components within ECS
		 * @tparam T The component type to check for
		 * @param entity The entity to check
		 * @return True if the entity has the component, false otherwise
		 */
		template<typename T>
		bool HasComponent(EntityID entity) const
		{
			return m_ComponentManager->HasComponent<T>(entity);
		}
	};
}
#include "ECS.tpp"
// 0x4B45414E
