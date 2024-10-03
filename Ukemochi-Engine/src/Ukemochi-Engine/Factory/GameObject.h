/* Start Header
*****************************************************************/
/*!
\file		GameObject.h
\par		Ukemochi
\author		Pek Jun Kai Gerald, p.junkaigerald, 2301334
\co-authors
\par		p.junkaigerald\@digipen.edu
\par		Course: CSD2400/CSD2401
\date		29/09/24
\brief		This file contains the declaration of the GameObject class. This class is responsible for
		creating and managing game objects.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include "PreCompile.h"
#include "../ECS/ECS.h"

namespace Ukemochi
{
	class GameObject
	{
		std::string m_Name;
		std::string m_Tag;

		EntityID m_InstanceID{};
	public:
		GameObject() = default;
		GameObject(EntityID id, std::string name = "GameObject", std::string tag = "Default") : m_Name(name), m_Tag(tag), m_InstanceID(id) {};
		GameObject(const GameObject& other) = default;
		GameObject(GameObject&& other) noexcept = default;
		GameObject& operator=(const GameObject& other);
		~GameObject() = default;

		template<typename T>
		void AddComponent(T component) { ECS::GetInstance().AddComponent(m_InstanceID, component); }

		template <typename T>
		T& GetComponent() { return ECS::GetInstance().GetComponent<T>(m_InstanceID); }

		template<typename T>
		void RemoveCoponent() { ECS::GetInstance().RemoveComponent<T>(m_InstanceID); }

		EntityID GetInstanceID();
	};
}