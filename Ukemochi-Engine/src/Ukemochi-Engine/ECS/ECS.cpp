/* Start Header ************************************************************************/
/*!
\file       ECS.cpp
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date       Sep 15, 2024
\brief      This file contains the definition of the member function of ECS.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

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

	void ECS::ReloadEntityManager()
	{
		m_EntityManager.reset(new EntityManager());
	}

	EntityID ECS::CloneEntity(EntityID entity)
	{
		EntityID newEntity = m_EntityManager->CreateEntity();

		SignatureID originalSignature = m_EntityManager->GetSignature(entity);
		m_EntityManager->SetSignature(newEntity, originalSignature);

		// TODO: Keep updating components as the list grows
		auto transform = m_ComponentManager->GetComponentType<Transform>();
		auto rigidbody2D = m_ComponentManager->GetComponentType<Rigidbody2D>();
		auto boxCollider = m_ComponentManager->GetComponentType<BoxCollider2D>();
		auto cirlceCollider = m_ComponentManager->GetComponentType<CircleCollider2D>();
		auto convexCollider = m_ComponentManager->GetComponentType<ConvexCollider2D>();
		auto spriteRenderer = m_ComponentManager->GetComponentType<SpriteRender>();
		auto script = m_ComponentManager->GetComponentType<Script>();
		//auto button = m_ComponentManager->GetComponentType<Button>();

		// Iterate through all possible components
		for (ComponentTypeID i = 0; i < MAX_COMPONENTS; ++i)
		{
			if (originalSignature.test(i))
			{
				// TODO: Keep updating components as the list grows
				if (i == transform)
				{
					Transform originalComponent = m_ComponentManager->GetComponent<Transform>(entity);
					AddComponent<Transform>(newEntity, originalComponent);
				}
				else if (i == rigidbody2D)
				{
					Rigidbody2D originalComponent = m_ComponentManager->GetComponent<Rigidbody2D>(entity);
					AddComponent<Rigidbody2D>(newEntity, originalComponent);
				}
				else if (i == cirlceCollider)
				{
					CircleCollider2D originalComponent = m_ComponentManager->GetComponent<CircleCollider2D>(entity);
					AddComponent<CircleCollider2D>(newEntity, originalComponent);
				}
				else if (i == boxCollider)
				{
					BoxCollider2D originalComponent = m_ComponentManager->GetComponent<BoxCollider2D>(entity);
					AddComponent<BoxCollider2D>(newEntity, originalComponent);
				}
				else if(i == convexCollider)
				{
					ConvexCollider2D originalComponent = m_ComponentManager->GetComponent<ConvexCollider2D>(entity);
					AddComponent<ConvexCollider2D>(newEntity, originalComponent);
				}
				else if (i == spriteRenderer)
				{
					SpriteRender originalComponent = m_ComponentManager->GetComponent<SpriteRender>(entity);
					AddComponent<SpriteRender>(newEntity, originalComponent);
				}
				else if (i == script)
				{
					Script originalComponent = m_ComponentManager->GetComponent<Script>(entity);
					AddComponent<Script>(newEntity, originalComponent);
				}
				//else if(i == button)
				//{
				//	//Button originalComponent = m_ComponentManager->GetComponent<Button>(entity);
				//	//AddComponent<Button>(newEntity, originalComponent);
				//}
				else
				{
					UME_ENGINE_WARN("Missing component type in CloneEntity");
				}
			}
		}

		return newEntity;
	}

	unsigned long int ECS::GetLivingEntityCount() const
	{
		return m_EntityManager->GetLivingEntityCount();
	}
}
// 0x4B45414E