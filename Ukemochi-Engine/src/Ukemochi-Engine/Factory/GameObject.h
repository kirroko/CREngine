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

namespace Ukemochi
{
    class GameObject
    {
        std::string m_Name;
        std::string m_Tag;

        EntityID m_InstanceID{};
        
        bool m_isActive = true;
    public:
        GameObject() = default;

        /**
         * @brief Construct a new GameObject object
         * 
         * @param id The entity ID
         * @param name The name of the game object
         * @param tag The tag of the game object
         */
        GameObject(EntityID id, std::string name = "GameObject", std::string tag = "Default");

        GameObject(const GameObject& other) = default;
        GameObject(GameObject&& other) noexcept = default;

        /**
         * @brief Copy constructor
         * 
         * @param other The game object to copy
         */
        GameObject& operator=(const GameObject& other);

        /**
         * @brief Add a component to the GameObject
         * 
         * @tparam T The component to add
         * @param component The component to add
         */
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

        /**
         * @brief Get a component from the GameObject
         * 
         * @tparam T The component to get
         * @return T& The component
         */
        template <typename T>
        T& GetComponent() { return ECS::GetInstance().GetComponent<T>(m_InstanceID); }

        /**
         * @brief Remove a component from the GameObject
         * 
         * @tparam T The component to remove
         */
        template <typename T>
        void RemoveComponent() const { ECS::GetInstance().RemoveComponent<T>(m_InstanceID); }

        /**
         * @brief Check if the GameObject has a component
         * 
         * @tparam T The component to check
         * @return true If the GameObject has the component
         * @return false If the GameObject does not have the component
         */
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

        /*!***********************************************************************
        \brief
         Sets the name of the object.

        \param[in] name
         A constant reference to a std::string representing the new name to be assigned to the object.

        *************************************************************************/
        void SetName(const std::string& name) { m_Name = name; }

        /**
         * @brief This function returns the tag of the object, also as a constant reference.
         * @return The tag of the object as a constant reference to a std::string.
         */
        const std::string& GetTag() const { return m_Tag; }

        /**
         * @brief This function sets the tag of the object.
         * @param tag A constant reference to a std::string representing the new tag to be assigned to the object.
         */
        void SetTag(const std::string& tag) { m_Tag = tag; }

        /**
         * @brief Get the active state of the gameobject
         * @return bool the active state of the game object
         */
        bool GetActive() const;

        /**
         * @brief Set the active state of the game object
         * @param active the active state of the game object
         */
        void SetActive(const bool active) { m_isActive = active; }
    };
};
