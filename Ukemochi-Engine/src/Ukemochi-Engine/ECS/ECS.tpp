/* Start Header
*****************************************************************/
/*!
\file	ECS.tpp
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
#include "ECS.h"

namespace Ukemochi
{
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