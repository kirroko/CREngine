#pragma once
#include "PreCompile.h"
#include "../ECS/ECS.h"

namespace Ukemochi
{
	class GameObject
	{
		std::string m_Name;
		std::string m_Tag;

		EntityID m_InstanceID;
	public:
		GameObject(EntityID id, std::string name = "GameObject", std::string tag = "Default") : m_Name(name), m_Tag(tag), m_InstanceID(id) {};

		template<typename T>
		void AddComponent(T component) { ECS::GetInstance().AddComponent(m_InstanceID, component); }

		template <typename T>
		T& GetComponent() { ECS::GetInstance().GetComponent(m_InstanceID); }

		template<typename T>
		void RemoveCoponent() { ECS::GetInstance().RemoveComponent<T>(m_InstanceID); }

		EntityID GetInstanceID();
	};
}