/* Start Header
*****************************************************************/
/*!
\file	InternalCalls.h
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	1/11/24
\brief	This file is forward declaring the internal calls from C# to C++

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include <mono/metadata/object.h> // for MonoObject

#include "Ukemochi-Engine/Factory/GameObjectManager.h"
#include "Ukemochi-Engine/Input/Input.h"

namespace Ukemochi
{
    namespace InternalCalls
    {
        /**
         * \brief Retrieves a managed MonoObject instance of a GameObject by its ID.
         * \param id The unique identifier of the GameObject.
         * \param objectName The name of the GameObject.
         * \return A MonoObject pointer representing the managed instance of the GameObject.
         */
        // EXTERN_C UME_API inline MonoObject* GetMonoObject(const uint64_t id, MonoString* objectName)
        // {
        //     const auto& go = GameObjectManager::GetInstance().GetGO(id);
        //     return go.GetManagedInstance(mono_string_to_utf8(objectName));
        // }


        EXTERN_C UME_API inline void GetObjectByTag(MonoString* tag, uint64_t* id)
        {
            auto* go = GameObjectManager::GetInstance().GetGOByTag(ScriptingEngine::MonoStringToUTF8(tag));
            *id = go->GetInstanceID();
        }

        /**
         * \brief Adds a component to a GameObject by its ID and component type name.
         * \param id The unique identifier of the GameObject.
         * \param typeName The name of the component type to add.
         */
        // EXTERN_C UME_API inline void AddComponent(uint64_t id, MonoObject* component, MonoString* typeName)
        // {
        //     auto& go = GameObjectManager::GetInstance().GetGO(id);
        //
        //     std::string typeNameStr = mono_string_to_utf8(typeName);
        //     auto it = GameObjectManager::componentRegistry.find(typeNameStr);
        //     if (it != GameObjectManager::componentRegistry.end())
        //     {
        //         it->second(go, component);
        //     }
        //     else
        //     {
        //         UME_ENGINE_WARN("Component type '{0}' not found in registry", typeNameStr); // check registry GOmanager
        //     }
        // }

        EXTERN_C UME_API inline bool HasComponent(uint64_t id, MonoReflectionType* componentType)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);

            MonoType* managedType = mono_reflection_type_get_type(componentType);
            UME_ENGINE_ASSERT(GameObjectManager::s_EntityHasCompoentFuncs.find(managedType) != GameObjectManager::s_EntityHasCompoentFuncs.end(),
                              "Component type not found in registry")
            return GameObjectManager::s_EntityHasCompoentFuncs.at(managedType)(id);
        }

        EXTERN_C UME_API inline bool GetKey(int32_t key)
        {
            return Input::IsKeyPressed(key);
        }

        EXTERN_C UME_API inline bool GetKeyDown(int32_t key)
        {
            return Input::IsKeyTriggered(key);
        }

        EXTERN_C UME_API inline void LogMessage(MonoString* message)
        {
            UME_ENGINE_INFO("Ukemochi log: {0}", std::string(mono_string_to_utf8(message)));
        }

        EXTERN_C UME_API inline void SetTransformPosition(uint64_t id, float* x, float* y)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& transform = go.GetComponent<Transform>();
            transform.position = Vec2(*x, *y);
        }

        EXTERN_C UME_API inline void GetTransformPosition(uint64_t id, float* x, float* y)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& transform = go.GetComponent<Transform>();
            *x = transform.position.x;
            *y = transform.position.y;
        }

        // EXTERN_C UME_API inline void SetTransformRotation(uint64_t id, float x, float y)
        // {
        //     // TODO: Implement rotation
        //     // auto& go = GameObjectManager::GetInstance().GetGO(id);
        //     // auto& transform = go.GetComponent<Transform>();
        //     // transform.rotation = x;
        // }

        EXTERN_C UME_API inline void GetTransformRotation(uint64_t id, float* x, float* y)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& transform = go.GetComponent<Transform>();
            *x = transform.rotation;
            *y = transform.rotation;
        }

        EXTERN_C UME_API inline void SetTransformScale(uint64_t id, float x, float y)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& transform = go.GetComponent<Transform>();
            transform.scale = Vec2(x, y);
        }

        EXTERN_C UME_API inline void GetTransformScale(uint64_t id, float* x, float* y)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& transform = go.GetComponent<Transform>();
            *x = transform.scale.x;
            *y = transform.scale.y;
        }

        EXTERN_C UME_API inline void SetRigidbodyVelocity(uint64_t id, float x, float y)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go.GetComponent<Rigidbody2D>();
            rb.velocity = Vec2(x, y);
        }

        EXTERN_C UME_API inline void GetRigidbodyVelocity(uint64_t id, float* x, float* y)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go.GetComponent<Rigidbody2D>();
            *x = rb.velocity.x;
            *y = rb.velocity.y;
        }

        EXTERN_C UME_API inline void SetRigidbodyAcceleration(uint64_t id, float x, float y)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go.GetComponent<Rigidbody2D>();
            rb.acceleration = Vec2(x, y);
        }

        EXTERN_C UME_API inline void GetRigidbodyAcceleration(uint64_t id, float* x, float* y)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go.GetComponent<Rigidbody2D>();
            *x = rb.acceleration.x;
            *y = rb.acceleration.y;
        }

        EXTERN_C UME_API inline void SetRigidbodyForce(uint64_t id, float x, float y)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go.GetComponent<Rigidbody2D>();
            rb.force = Vec2(x, y);
        }

        EXTERN_C UME_API inline void GetRigidbodyForce(uint64_t id, float* x, float* y)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go.GetComponent<Rigidbody2D>();
            *x = rb.force.x;
            *y = rb.force.y;
        }

        EXTERN_C UME_API inline void SetRigidbodyMass(uint64_t id, float mass)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go.GetComponent<Rigidbody2D>();
            rb.mass = mass;
            rb.inverse_mass = 1.0f / mass;
        }

        EXTERN_C UME_API inline void SetRigidbodyInverseMass(uint64_t id, float inverseMass)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go.GetComponent<Rigidbody2D>();
            rb.inverse_mass = inverseMass;
            rb.mass = 1.0f / inverseMass;
        }

        EXTERN_C UME_API inline void SetRigidbodyDrag(uint64_t id, float drag)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go.GetComponent<Rigidbody2D>();
            rb.linear_drag = drag;
        }

        EXTERN_C UME_API inline void SetRigidbodyAngle(uint64_t id, float angle)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go.GetComponent<Rigidbody2D>();
            rb.angle = angle;
        }

        EXTERN_C UME_API inline void SetRigidbodyAngularVelocity(uint64_t id, float velocity)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go.GetComponent<Rigidbody2D>();
            rb.angular_velocity = velocity;
        }

        EXTERN_C UME_API inline void SetRigidbodyTorque(uint64_t id, float torque)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go.GetComponent<Rigidbody2D>();
            rb.torque = torque;
        }

        EXTERN_C UME_API inline void SetRigidbodyInertiaMass(uint64_t id, float inertiaMass)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go.GetComponent<Rigidbody2D>();
            rb.inertia_mass =inertiaMass;
            rb.inv_inertia_mass = 1.0f / inertiaMass;
        }

        EXTERN_C UME_API inline void SetRigidbodyInverseInertiaMass(uint64_t id, float inverseInertiaMass)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go.GetComponent<Rigidbody2D>();
            rb.inv_inertia_mass = inverseInertiaMass;
            rb.inertia_mass = 1.0f / inverseInertiaMass;
        }

        EXTERN_C UME_API inline void SetRigidbodyAngularDrag(uint64_t id, float drag)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go.GetComponent<Rigidbody2D>();
            rb.angular_drag =drag;
        }

        EXTERN_C UME_API inline void SetRigidbodyUseGravity(uint64_t id, bool useGravity)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go.GetComponent<Rigidbody2D>();
            rb.use_gravity = useGravity;
        }

        EXTERN_C UME_API inline void SetRigidbodyIsKinematic(uint64_t id, bool isKinematic)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go.GetComponent<Rigidbody2D>();
            rb.is_kinematic = isKinematic;
        }

        EXTERN_C UME_API inline void SetSpriteRenderFlipX(uint64_t id, bool flipX)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& sr = go.GetComponent<SpriteRender>();
            sr.flipX = flipX;
        }

        EXTERN_C UME_API inline void SetSpriteRenderFlipY(uint64_t id, bool flipY)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);
            auto& sr = go.GetComponent<SpriteRender>();
            sr.flipY = flipY;
        }
    }
}
