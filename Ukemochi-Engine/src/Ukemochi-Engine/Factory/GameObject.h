/* Start Header ************************************************************************/
/*!
\file       GameObject.h
\author     Pek Jun Kai Gerald, p.junkaigerald, 2301334, p.junkaigerald\@digipen.edu (50%)
\co-authors Wong Jun Yu, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu (50%)
\date       Sept 29, 2024
\brief      This file contains the declaration of the GameObject class. This class is responsible for
        creating and managing game objects. This applies to the C# side as well. It reflects the gameobject instance
        on the C# side of things.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include "PreCompile.h"

#include "../ECS/ECS.h"

#include <typeinfo>

namespace Ukemochi
{
    class GameObject
    {
        std::string m_Name;
        std::string m_Tag;

        EntityID m_InstanceID{};

    public:
        GameObject() = default;

        GameObject(EntityID id, std::string name = "GameObject", std::string tag = "Default");

        GameObject(const GameObject& other) = default;
        GameObject(GameObject&& other) noexcept = default;
        GameObject& operator=(const GameObject& other);


        template <typename T>
        void AddComponent(T component)
        {
            std::string type = typeid(T).name();
            size_t pos = type.find_first_of("::");
            if (pos != std::string::npos)
                type = type.substr(pos + 2);

            UME_ENGINE_TRACE("GO ({0}) Adding Component: {1}", m_InstanceID, type);
            ECS::GetInstance().AddComponent(m_InstanceID, component);
        }

        template <typename T>
        T& GetComponent() { return ECS::GetInstance().GetComponent<T>(m_InstanceID); }

        template <typename T>
        void RemoveComponent() const { ECS::GetInstance().RemoveComponent<T>(m_InstanceID); }

        template <typename T>
        bool HasComponent() const { return ECS::GetInstance().HasComponent<T>(m_InstanceID); }

        /**
         * @brief returns the instance ID of the GameObject
         * @return The instance ID of the GameObject 
         */
        EntityID GetInstanceID() const;

        /**
         * @brief This function returns the name of the object as a constant reference to a std::string.
         * @return The name of the object as a constant reference to a std::string.
         */
        const std::string& GetName() const { return m_Name; }

        void SetName(const std::string& name) { m_Name = name; }

        /**
         * @brief This function returns the tag of the object, also as a constant reference.
         * @return The tag of the object as a constant reference to a std::string.
         */
        const std::string& GetTag() const { return m_Tag; }

        void SetTag(const std::string& tag) { m_Tag = tag; }
    };
};
