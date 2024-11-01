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
		creating and managing game objects. This applies to the C# side as well. It reflects the gameobject instance
		on the C# side of things

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include "PreCompile.h"

#include "../ECS/ECS.h"
#include "Ukemochi-Engine/Logic/Logic.h"

#include <mono/metadata/object-forward.h>
#include <typeinfo>

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

        GameObject(EntityID id, std::string name = "GameObject", std::string tag = "Default");

        GameObject(const GameObject& other) = default;
        GameObject(GameObject&& other) noexcept = default;
        GameObject& operator=(const GameObject& other);
        ~GameObject() = default;


        template <typename T>
        void AddComponent(T component)
        {
            std::string type = typeid(T).name();
            size_t pos = type.find_first_of("::");
            if (pos != std::string::npos)
                type = type.substr(pos + 2);
            ECS::GetInstance().AddComponent(m_InstanceID, component);
            // std::filesystem::path filePath(scriptObj.scriptPath);
            // std::string fileName = filePath.stem().string();

            // We Instantiate the C# component
            // We probably don't need to check if the component is already in the map since the above code will stop
            if (strcmp(type.c_str(), "Script") == 0)
            {
                auto& script = ECS::GetInstance().GetComponent<Script>(m_InstanceID);
                m_ManagedComponents[type] = ScriptingEngine::GetInstance().InstantiateClientClass(script.scriptName);
            }
            else
                m_ManagedComponents[type] = ScriptingEngine::GetInstance().InstantiateClass(type);

            // In the C# side, we need to add the instance created from C++ to the GameObject instance container
            // TODO: Make sure the C# wrapper has methods to add components
            void* param[] = {m_ManagedComponents.at(type)};
            ScriptingEngine::GetInstance().InvokeMethod(m_ManagedInstance,
                                                        "Add" + type + "Component", param, 1);
        }

        template <typename T>
        T& GetComponent() { return ECS::GetInstance().GetComponent<T>(m_InstanceID); }

        template <typename T>
        void RemoveCoponent() { ECS::GetInstance().RemoveComponent<T>(m_InstanceID); }

        /**
         * @brief Set the managed instance of the component
         * @param instance The managed instance of the component 
         */
        void SetManagedComponentInstance(MonoObject* instance, const std::string& typeName);

        /**
         * @brief Returns back the managed instance of the GameObject (C# GameObject class)
         * @return The managed instance of the GameObject
         */
        MonoObject* GetManagedInstance() const;

        /**
         * @brief Returns the managed instance of the components inside GameObject
         * @param objectName the name of the class object (Transform, Rigidbody2D, etc.)
         * @return The managed instance of the component
         */
        MonoObject* GetManagedInstance(const std::string& objectName) const;

        /**
         * @brief returns the instance ID of the GameObject
         * @return The instance ID of the GameObject 
         */
        EntityID GetInstanceID();
    };
}
