/* Start Header
*****************************************************************/
/*!
\file ECS.cpp
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par junyukean.wong\@digipen.edu
\date 15/09/24
\brief This file contains the definition of the member function of ECS

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "PreCompile.h"
#include "ECS.h"

namespace Ukemochi
{
	void ECS::Init()
	{
		m_ComponentManager = std::make_unique<ComponentManager>();
		m_EntityManager = std::make_unique<EntityManager>();
		m_SystemManager = std::make_unique<SystemManager>();
	}

	EntityID ECS::CreateEntity()
	{
		return m_EntityManager->CreateEntity();
	}

	void ECS::DestroyEntity(EntityID entity)
	{
		m_EntityManager->DestroyEntity(entity);
		m_ComponentManager->EntityDestroyed(entity);
		m_SystemManager->EntityDestroyed(entity);
	}

	template <typename T>
	void ECS::RegisterComponent()
	{
		m_ComponentManager->RegisterComponent<T>();
	}

	template <typename T>
	void ECS::AddComponent(EntityID entity, T component)
	{
		m_ComponentManager->AddComponent<T>(entity, component);

		auto signature = m_EntityManager->GetSignature(entity);
		signature.set(m_ComponentManager->GetComponentType<T>(), true);
		m_EntityManager->SetSignature(entity, signature);

		m_SystemManager->EntitySignatureChanged(entity, signature);
	}

	template <typename T>
	void ECS::RemoveComponent(EntityID entity)
	{
		m_ComponentManager->RemoveComponent<T>(entity);

		auto signature = m_EntityManager->GetSignature(entity);
		signature.set(m_ComponentManager->GetComponentType<T>(), false);
		m_EntityManager->SetSignature(entity, signature);

		m_SystemManager->EntitySignatureChanged(entity, signature);
	}

	template <typename T>
	T& ECS::GetComponent(EntityID entity)
	{
		return m_ComponentManager->GetComponent<T>(entity);
	}

	template <typename T>
	ComponentTypeID ECS::GetComponentType()
	{
		return m_ComponentManager->GetComponentType<T>();
	}

	template <typename T>
	std::shared_ptr<T> ECS::RegisterSystem()
	{
		return m_SystemManager->RegisterSystem<T>();
	}

	template <typename T>
	void ECS::SetSystemSignature(SignatureID signature)
	{
		m_SystemManager->SetSystemSignature<T>(signature);
	}
}