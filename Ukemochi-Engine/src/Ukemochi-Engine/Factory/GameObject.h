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

        MonoObject* m_ManagedInstance = nullptr;
        // C# instance // TODO: To be refactored to let mono_gchandle_get_target() handle this
        MonoGCHandle m_ManagedInstanceHandle; // Handle to the C# instance 

        std::unordered_map<std::string, MonoObject*> m_ManagedComponents;
        // C# components // TODO: To be refactored to let mono_gchandle_get_target() handle this
        std::unordered_map<std::string, MonoGCHandle> m_ManagedComponentsHandle; // Handle to the C# components

        EntityID m_InstanceID{};

    public:
        GameObject() = default;

        GameObject(EntityID id, std::string name = "GameObject", std::string tag = "Default");

        GameObject(const GameObject& other) = default;
        GameObject(GameObject&& other) noexcept = default;
        GameObject& operator=(const GameObject& other);
        ~GameObject();


        template <typename T>
        void AddComponent(T component)
        {
            // For Script component
            std::string type = typeid(T).name();
            size_t pos = type.find_first_of("::");
            if (pos != std::string::npos)
                type = type.substr(pos + 2);

            UME_ENGINE_TRACE("GO ({0}) Adding Component: {1}", m_InstanceID, type);
            ECS::GetInstance().AddComponent(m_InstanceID, component);

            // We Instantiate the C# component
            // We probably don't need to check if the component is already in the map since the above code will stop
            if (strcmp(type.c_str(), "Script") == 0)
            {
                auto& script = ECS::GetInstance().GetComponent<Script>(m_InstanceID);
                MonoObject* scriptInstance = static_cast<MonoObject*>(script.instance);
                if (!scriptInstance)
                    UME_ENGINE_ASSERT(false, "Failed to cast Script component");

                ScriptingEngine::GetInstance().SetMonoPropertyValue(static_cast<MonoObject*>(script.instance),
                                                                    "gameObject", m_ManagedInstance);

                m_ManagedComponents[type] = static_cast<MonoObject*>(script.instance); // Instantiate at Factory
                m_ManagedComponentsHandle[type] = mono_gchandle_new_v2(static_cast<MonoObject*>(script.instance), true);

                void* param[] = {m_ManagedComponents.at(type)};
                ScriptingEngine::GetInstance().InvokeMethod(m_ManagedInstance,
                                                            "Add" + type + "Component", param, 1);
            }
            else if (strcmp(type.c_str(), "Transform") == 0)
            {
                MonoObject* transform = ScriptingEngine::GetInstance().InstantiateClass(type);
                ScriptingEngine::GetInstance().SetMonoPropertyValue(transform, "gameObject", m_ManagedInstance);
                auto& transformCast = ECS::GetInstance().GetComponent<Transform>(m_InstanceID);

                ScriptingEngine::GetInstance().SetVector2Property(transform, "Position", transformCast.position.x,
                                                                  transformCast.position.y);
                // ScriptingEngine::GetInstance().SetVector2Property(transform, "Rotation", transformCast->rotation); // TODO: Rotation not completed
                ScriptingEngine::GetInstance().SetVector2Property(transform, "Scale", transformCast.scale.x,
                                                                  transformCast.scale.y);

                m_ManagedComponents[type] = transform;
                m_ManagedComponentsHandle[type] = mono_gchandle_new_v2(transform, true);

                void* param[] = { m_ManagedComponents.at(type)};
                ScriptingEngine::GetInstance().InvokeMethod(m_ManagedInstance,
                                                            "Add" + type + "Component", param, 1);
            }
            else if (strcmp(type.c_str(), "Rigidbody2D") == 0)
            {
                MonoObject* rigidbody = ScriptingEngine::GetInstance().InstantiateClass(type);
                ScriptingEngine::GetInstance().SetMonoPropertyValue(rigidbody, "gameObject", m_ManagedInstance);
                auto& rigidbodyCast = ECS::GetInstance().GetComponent<Rigidbody2D>(m_InstanceID);

                ScriptingEngine::GetInstance().SetVector2Property(rigidbody, "Force", rigidbodyCast.force.x,
                                                                  rigidbodyCast.force.y);
                ScriptingEngine::GetInstance().SetVector2Property(rigidbody, "Velocity", rigidbodyCast.velocity.x,
                                                                  rigidbodyCast.velocity.y);
                ScriptingEngine::GetInstance().SetVector2Property(rigidbody, "Acceleration",
                                                                  rigidbodyCast.acceleration.x,
                                                                  rigidbodyCast.acceleration.y);
                ScriptingEngine::GetInstance().SetMonoPropertyValue(rigidbody, "Mass", &rigidbodyCast.mass);
                ScriptingEngine::GetInstance().SetMonoPropertyValue(rigidbody, "InverseMass",
                                                                    &rigidbodyCast.inverse_mass);
                ScriptingEngine::GetInstance().
                    SetMonoPropertyValue(rigidbody, "UseGravity", &rigidbodyCast.use_gravity);
                ScriptingEngine::GetInstance().SetMonoPropertyValue(rigidbody, "IsKinematic",
                                                                    &rigidbodyCast.is_kinematic);

                m_ManagedComponents[type] = rigidbody;
                m_ManagedComponentsHandle[type] = mono_gchandle_new_v2(rigidbody, true);

                void* param[] = {m_ManagedComponents.at(type)};
                ScriptingEngine::GetInstance().InvokeMethod(m_ManagedInstance,
                                                            "Add" + type + "Component", param, 1);
            }
            else
            {
                UME_ENGINE_INFO("!! Item is still WIP");
                // m_ManagedComponents[type] = ScriptingEngine::GetInstance().InstantiateClass(type);
            }

            // In the C# side, we need to add the instance created from C++ to the GameObject instance container
            // TODO: Make sure the C# wrapper has methods to add components
            // void* param[] = {&m_ManagedComponents.at(type)};
            // ScriptingEngine::GetInstance().InvokeMethod(m_ManagedInstance,
            //                                             "Add" + type + "Component", param, 1);
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
