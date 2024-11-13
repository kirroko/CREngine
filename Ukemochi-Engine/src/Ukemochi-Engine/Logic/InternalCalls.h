/* Start Header ************************************************************************/
/*!
\file       InternalCalls.h
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date       Nov 1, 2024
\brief      This file is forward declaring the internal calls from C# to C++.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include <mono/metadata/object.h> // for MonoObject

#include "Scripting.h"
#include "Ukemochi-Engine/Factory/GameObjectManager.h"
#include "Ukemochi-Engine/Input/Input.h"

namespace Ukemochi
{
    namespace InternalCalls
    {
        EXTERN_C UME_API inline void GetObjectByTag(MonoString* tag, uint64_t* id)
        {
            auto* go = GameObjectManager::GetInstance().GetGOByTag(ScriptingEngine::MonoStringToUTF8(tag));
            if(go != nullptr)
            {
                *id = go->GetInstanceID();
                return;
            }
            UME_ENGINE_WARN("GameObject with tag '{0}' not found", ScriptingEngine::MonoStringToUTF8(tag));
            *id = 999999; // TODO: We'll come back to this another day, Handle no gameobject found
        }

        EXTERN_C UME_API inline bool HasComponent(uint64_t id, MonoReflectionType* componentType)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            UME_ENGINE_ASSERT(go, "GameObject not found")
            
            MonoType* managedType = mono_reflection_type_get_type(componentType);
            // TODO: Check if its a script its getting instead of a component
            UME_ENGINE_ASSERT(ScriptingEngine::s_EntityHasComponentFuncs.find(managedType) != ScriptingEngine::s_EntityHasComponentFuncs.end(),
                              "Component type not found in registry")
            auto tdebug = ScriptingEngine::s_EntityHasComponentFuncs.at(managedType)(id);
            return tdebug;
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
            UME_ENGINE_INFO("Ukemochi log: {0}", ScriptingEngine::MonoStringToUTF8(message));
        }

        EXTERN_C UME_API inline void LogWarning(MonoString* message)
        {
            UME_ENGINE_WARN("Ukemochi Log: {0}", ScriptingEngine::MonoStringToUTF8(message));
        }

        EXTERN_C UME_API inline void SetTransformPosition(uint64_t id, float* x, float* y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& transform = go->GetComponent<Transform>();
            transform.position = Vec2(*x, *y);
        }

        EXTERN_C UME_API inline void GetTransformPosition(uint64_t id, float* x, float* y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& transform = go->GetComponent<Transform>();
            *x = transform.position.x;
            *y = transform.position.y;
        }

        // EXTERN_C UME_API inline void SetTransformRotation(uint64_t id, float x, float y)
        // {
        //     // auto* go = GameObjectManager::GetInstance().GetGO(id);
        //     // auto& transform = go->GetComponent<Transform>();
        //     // transform.rotation = x;
        // }

        EXTERN_C UME_API inline void GetTransformRotation(uint64_t id, float* x, float* y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& transform = go->GetComponent<Transform>();
            *x = transform.rotation;
            *y = transform.rotation;
        }

        EXTERN_C UME_API inline void SetTransformScale(uint64_t id, float x, float y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& transform = go->GetComponent<Transform>();
            transform.scale = Vec2(x, y);
        }

        EXTERN_C UME_API inline void GetTransformScale(uint64_t id, float* x, float* y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& transform = go->GetComponent<Transform>();
            *x = transform.scale.x;
            *y = transform.scale.y;
        }

        EXTERN_C UME_API inline void SetRigidbodyVelocity(uint64_t id, float x, float y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.velocity = Vec2(x, y);
        }

        EXTERN_C UME_API inline void GetRigidbodyVelocity(uint64_t id, float* x, float* y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            *x = rb.velocity.x;
            *y = rb.velocity.y;
        }

        EXTERN_C UME_API inline void SetRigidbodyAcceleration(uint64_t id, float x, float y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.acceleration = Vec2(x, y);
        }

        EXTERN_C UME_API inline void GetRigidbodyAcceleration(uint64_t id, float* x, float* y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            *x = rb.acceleration.x;
            *y = rb.acceleration.y;
        }

        EXTERN_C UME_API inline void SetRigidbodyForce(uint64_t id, float x, float y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.force = Vec2(x, y);
        }

        EXTERN_C UME_API inline void GetRigidbodyForce(uint64_t id, float* x, float* y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            *x = rb.force.x;
            *y = rb.force.y;
        }

        EXTERN_C UME_API inline void SetRigidbodyMass(uint64_t id, float mass)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.mass = mass;
            rb.inverse_mass = 1.0f / mass;
        }

        EXTERN_C UME_API inline void SetRigidbodyInverseMass(uint64_t id, float inverseMass)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.inverse_mass = inverseMass;
            rb.mass = 1.0f / inverseMass;
        }

        EXTERN_C UME_API inline void SetRigidbodyDrag(uint64_t id, float drag)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.linear_drag = drag;
        }

        EXTERN_C UME_API inline void SetRigidbodyAngle(uint64_t id, float angle)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.angle = angle;
        }

        EXTERN_C UME_API inline void SetRigidbodyAngularVelocity(uint64_t id, float velocity)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.angular_velocity = velocity;
        }

        EXTERN_C UME_API inline void SetRigidbodyTorque(uint64_t id, float torque)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.torque = torque;
        }

        EXTERN_C UME_API inline void SetRigidbodyInertiaMass(uint64_t id, float inertiaMass)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.inertia_mass =inertiaMass;
            rb.inv_inertia_mass = 1.0f / inertiaMass;
        }

        EXTERN_C UME_API inline void SetRigidbodyInverseInertiaMass(uint64_t id, float inverseInertiaMass)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.inv_inertia_mass = inverseInertiaMass;
            rb.inertia_mass = 1.0f / inverseInertiaMass;
        }

        EXTERN_C UME_API inline void SetRigidbodyAngularDrag(uint64_t id, float drag)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.angular_drag =drag;
        }

        EXTERN_C UME_API inline void SetRigidbodyUseGravity(uint64_t id, bool useGravity)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.use_gravity = useGravity;
        }

        EXTERN_C UME_API inline void SetRigidbodyIsKinematic(uint64_t id, bool isKinematic)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.is_kinematic = isKinematic;
        }

        EXTERN_C UME_API inline void SetSpriteRenderFlipX(uint64_t id, bool flipX)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& sr = go->GetComponent<SpriteRender>();
            sr.flipX = flipX;
        }

        EXTERN_C UME_API inline void SetSpriteRenderFlipY(uint64_t id, bool flipY)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& sr = go->GetComponent<SpriteRender>();
            sr.flipY = flipY;
        }
    }
}
