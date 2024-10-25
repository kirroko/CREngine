/* Start Header
*****************************************************************/
/*!
\file	ECS.h
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	15/09/24
\brief	The ECS will coordinator between the Component, Entity and System managers

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once

#include "Component.h" // ComponentManager
#include "Entity.h"    // EntityManager
#include "Systems.h"   // SystemManager
#include "Components.h"

namespace Ukemochi
{
	class ECS
	{
		std::unique_ptr<ComponentManager> m_ComponentManager;
		std::unique_ptr<EntityManager> m_EntityManager;
		std::unique_ptr<SystemManager> m_SystemManager;

		ECS() = default;
		~ECS() = default;
		ECS(const ECS&) = delete;
		ECS& operator=(const ECS&) = delete;
	public:
		static ECS& GetInstance()
		{
			static ECS instance;
			return instance;
		}

		// Create pointers to each manager
		void Init();

		EntityID CreateEntity();

		void DestroyEntity(EntityID entity);

		EntityID CloneEntity(EntityID entity);

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
	};
}
#include "ECS.tpp"
// 0x4B45414E
