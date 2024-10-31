/* Start Header
*****************************************************************/
/*!
\file		GameObject.h
\par		Ukemochi
\author		Pek Jun Kai Gerald, p.junkaigerald, 2301334, p.junkaigerald\@digipen.edu (50%)
\co-authors Wong Jun Yu, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu (50%)
\par		Course: CSD2400/CSD2401
\date		29/09/24
\brief		This file contains the declaration of the GameObject class. This class is responsible for
		creating and managing game objects. This applies to the C# side as well

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include "PreCompile.h"

#include <mono/metadata/object-forward.h>

#include "../ECS/ECS.h"
#include "Ukemochi-Engine/Logic/Logic.h"

namespace Ukemochi
{
    class GameObject
    {
        std::string m_Name;
        std::string m_Tag;

        MonoObject* m_ManagedInstance = nullptr; // C# instance
        std::unordered_map<std::string, MonoObject*> m_ManagedComponents; // C# components

        EntityID m_InstanceID{};

    public:
        GameObject() = default;

        GameObject(EntityID id, std::string name = "GameObject", std::string tag = "Default") : m_Name(name),
            m_Tag(tag), m_InstanceID(id)
        {
        }

        GameObject(const GameObject& other) = default;
        GameObject(GameObject&& other) noexcept = default;
        GameObject& operator=(const GameObject& other);
        ~GameObject() = default;

        template <typename T>
        void AddComponent(T component)
        {
            ECS::GetInstance().AddComponent(m_InstanceID, component);
            // We Instantiate the C# component
            // We probably don't need to check if the component is already in the map since ECS takes care of that
            m_ManagedComponents[typeid(T).name()] = LogicSystem::GetMonoManager().InstantiateClass(typeid(T).name());
            // The C# gameobject instance needs to know that a component has been added
            // TODO: Make sure the C# wrapper has methods to add components 
            LogicSystem::GetMonoManager().InvokeMethod(m_ManagedInstance, "Add" + std::string(typeid(T).name()) + "Component", nullptr, 0);
        }

        template <typename T>
        T& GetComponent() { return ECS::GetInstance().GetComponent<T>(m_InstanceID); }

        template <typename T>
        void RemoveCoponent() { ECS::GetInstance().RemoveComponent<T>(m_InstanceID); }

        void SetManagedInstance(MonoObject* instance) { m_ManagedInstance = instance; }

        MonoObject* GetManagedInstance() const { return m_ManagedInstance; }

        EntityID GetInstanceID();
    };
}
