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
#include "Ukemochi-Engine/FrameController.h"
#include "Ukemochi-Engine/Factory/GameObjectManager.h"
#include "Ukemochi-Engine/Input/Input.h"

namespace Ukemochi
{
    namespace InternalCalls
    {
        /*!
         * \brief Retrieves a GameObject by its tag.
         * \param tag The tag of the GameObject.
         * \param id Output parameter to store the instance ID of the GameObject.
         */
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

        /*!
         * \brief Checks if a GameObject has a specific component.
         * \param id The instance ID of the GameObject.
         * \param componentType The type of the component to check for.
         * \return True if the GameObject has the component, false otherwise.
         */
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

        /*!
         * \brief Checks if a key is currently pressed.
         * \param key The key code to check.
         * \return True if the key is pressed, false otherwise.
         */
        EXTERN_C UME_API inline bool GetKey(int32_t key)
        {
            return Input::IsKeyPressed(key);
        }

        /*!
         * \brief Checks if a key was pressed down in the current frame.
         * \param key The key code to check.
         * \return True if the key was pressed down, false otherwise.
         */
        EXTERN_C UME_API inline bool GetKeyDown(int32_t key)
        {
            return Input::IsKeyTriggered(key);
        }

        /*!
         * \brief Checks if a mouse button is currently pressed.
         * \param button The mouse button code to check.
         * \return True if the mouse button is pressed, false otherwise.
         */
        EXTERN_C UME_API inline bool GetMouseButton(int32_t button)
        {
            return Input::IsMouseButtonPressed(button);
        }

        /*!
         * \brief Checks if a mouse button was pressed down in the current frame.
         * \param button The mouse button code to check.
         * \return True if the mouse button was pressed down, false otherwise.
         */
        EXTERN_C UME_API inline bool GetMouseButtonDown(int32_t button)
        {
            return Input::IsMouseButtonTriggered(button);
        }

        /*!
         * \brief Logs a message to the engine's log.
         * \param message The message to log.
         */
        EXTERN_C UME_API inline void LogMessage(MonoString* message)
        {
            UME_ENGINE_INFO("Ukemochi log: {0}", ScriptingEngine::MonoStringToUTF8(message));
        }

        /*!
         * \brief Logs a warning message to the engine's log.
         * \param message The warning message to log.
         */
        EXTERN_C UME_API inline void LogWarning(MonoString* message)
        {
            UME_ENGINE_WARN("Ukemochi Log: {0}", ScriptingEngine::MonoStringToUTF8(message));
        }

        /*!
         * \brief Sets the position of a GameObject's transform.
         * \param id The instance ID of the GameObject.
         * \param x The new x-coordinate of the position.
         * \param y The new y-coordinate of the position.
         */
        EXTERN_C UME_API inline void SetTransformPosition(uint64_t id, float* x, float* y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& transform = go->GetComponent<Transform>();
            transform.position = Vec3(*x, *y,0);
        }

        /*!
         * \brief Gets the position of a GameObject's transform.
         * \param id The instance ID of the GameObject.
         * \param x Output parameter to store the x-coordinate of the position.
         * \param y Output parameter to store the y-coordinate of the position.
         */
        EXTERN_C UME_API inline void GetTransformPosition(uint64_t id, float* x, float* y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& transform = go->GetComponent<Transform>();
            *x = transform.position.x;
            *y = transform.position.y;
        }

        /*!
         * \brief Gets the rotation of a GameObject's transform.
         * \param id The instance ID of the GameObject.
         * \param x Output parameter to store the rotation value.
         * \param y Output parameter to store the rotation value.
         */
        EXTERN_C UME_API inline void GetTransformRotation(uint64_t id, float* x, float* y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& transform = go->GetComponent<Transform>();
            *x = transform.rotation;
            *y = transform.rotation;
        }

        /*!
         * \brief Sets the scale of a GameObject's transform.
         * \param id The instance ID of the GameObject.
         * \param x The new x-scale.
         * \param y The new y-scale.
         */
        EXTERN_C UME_API inline void SetTransformScale(uint64_t id, float x, float y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& transform = go->GetComponent<Transform>();
            transform.scale = Vec2(x, y);
        }

        /*!
         * \brief Gets the scale of a GameObject's transform.
         * \param id The instance ID of the GameObject.
         * \param x Output parameter to store the x-scale.
         * \param y Output parameter to store the y-scale.
         */
        EXTERN_C UME_API inline void GetTransformScale(uint64_t id, float* x, float* y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& transform = go->GetComponent<Transform>();
            *x = transform.scale.x;
            *y = transform.scale.y;
        }

        /*!
         * \brief Sets the velocity of a GameObject's Rigidbody2D.
         * \param id The instance ID of the GameObject.
         * \param x The new x-velocity.
         * \param y The new y-velocity.
         */
        EXTERN_C UME_API inline void SetRigidbodyVelocity(uint64_t id, float x, float y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.velocity = Vec2(x, y);
        }

        /*!
         * \brief Gets the velocity of a GameObject's Rigidbody2D.
         * \param id The instance ID of the GameObject.
         * \param x Output parameter to store the x-velocity.
         * \param y Output parameter to store the y-velocity.
         */
        EXTERN_C UME_API inline void GetRigidbodyVelocity(uint64_t id, float* x, float* y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            *x = rb.velocity.x;
            *y = rb.velocity.y;
        }

        /*!
         * \brief Sets the acceleration of a GameObject's Rigidbody2D.
         * \param id The instance ID of the GameObject.
         * \param x The new x-acceleration.
         * \param y The new y-acceleration.
         */
        EXTERN_C UME_API inline void SetRigidbodyAcceleration(uint64_t id, float x, float y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.acceleration = Vec2(x, y);
        }

        /*!
         * \brief Gets the acceleration of a GameObject's Rigidbody2D.
         * \param id The instance ID of the GameObject.
         * \param x Output parameter to store the x-acceleration.
         * \param y Output parameter to store the y-acceleration.
         */
        EXTERN_C UME_API inline void GetRigidbodyAcceleration(uint64_t id, float* x, float* y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            *x = rb.acceleration.x;
            *y = rb.acceleration.y;
        }

        /*!
         * \brief Sets the force of a GameObject's Rigidbody2D.
         * \param id The instance ID of the GameObject.
         * \param x The new x-force.
         * \param y The new y-force.
         */
        EXTERN_C UME_API inline void SetRigidbodyForce(uint64_t id, float x, float y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.force = Vec2(x, y);
        }

        /*!
         * \brief Gets the force of a GameObject's Rigidbody2D.
         * \param id The instance ID of the GameObject.
         * \param x Output parameter to store the x-force.
         * \param y Output parameter to store the y-force.
         */
        EXTERN_C UME_API inline void GetRigidbodyForce(uint64_t id, float* x, float* y)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            *x = rb.force.x;
            *y = rb.force.y;
        }

        /*!
         * \brief Sets the mass of a GameObject's Rigidbody2D.
         * \param id The instance ID of the GameObject.
         * \param mass The new mass.
         */
        EXTERN_C UME_API inline void SetRigidbodyMass(uint64_t id, float mass)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.mass = mass;
            rb.inverse_mass = 1.0f / mass;
        }

        /*!
         * \brief Sets the inverse mass of a GameObject's Rigidbody2D.
         * \param id The instance ID of the GameObject.
         * \param inverseMass The new inverse mass.
         */
        EXTERN_C UME_API inline void SetRigidbodyInverseMass(uint64_t id, float inverseMass)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.inverse_mass = inverseMass;
            rb.mass = 1.0f / inverseMass;
        }

        /*!
         * \brief Sets the linear drag of a GameObject's Rigidbody2D.
         * \param id The instance ID of the GameObject.
         * \param drag The new linear drag.
         */
        EXTERN_C UME_API inline void SetRigidbodyDrag(uint64_t id, float drag)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.linear_drag = drag;
        }

        /*!
         * \brief Sets the angle of a GameObject's Rigidbody2D.
         * \param id The instance ID of the GameObject.
         * \param angle The new angle.
         */
        EXTERN_C UME_API inline void SetRigidbodyAngle(uint64_t id, float angle)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.angle = angle;
        }

        /*!
         * \brief Sets the angular velocity of a GameObject's Rigidbody2D.
         * \param id The instance ID of the GameObject.
         * \param velocity The new angular velocity.
         */
        EXTERN_C UME_API inline void SetRigidbodyAngularVelocity(uint64_t id, float velocity)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.angular_velocity = velocity;
        }

        /*!
         * \brief Sets the torque of a GameObject's Rigidbody2D.
         * \param id The instance ID of the GameObject.
         * \param torque The new torque.
         */
        EXTERN_C UME_API inline void SetRigidbodyTorque(uint64_t id, float torque)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.torque = torque;
        }

        /*!
         * \brief Sets the inertia mass of a GameObject's Rigidbody2D.
         * \param id The instance ID of the GameObject.
         * \param inertiaMass The new inertia mass.
         */
        EXTERN_C UME_API inline void SetRigidbodyInertiaMass(uint64_t id, float inertiaMass)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.inertia_mass = inertiaMass;
            rb.inv_inertia_mass = 1.0f / inertiaMass;
        }

        /*!
         * \brief Sets the inverse inertia mass of a GameObject's Rigidbody2D.
         * \param id The instance ID of the GameObject.
         * \param inverseInertiaMass The new inverse inertia mass.
         */
        EXTERN_C UME_API inline void SetRigidbodyInverseInertiaMass(uint64_t id, float inverseInertiaMass)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.inv_inertia_mass = inverseInertiaMass;
            rb.inertia_mass = 1.0f / inverseInertiaMass;
        }

        /*!
         * \brief Sets the angular drag of a GameObject's Rigidbody2D.
         * \param id The instance ID of the GameObject.
         * \param drag The new angular drag.
         */
        EXTERN_C UME_API inline void SetRigidbodyAngularDrag(uint64_t id, float drag)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.angular_drag = drag;
        }

        /*!
         * \brief Sets whether a GameObject's Rigidbody2D uses gravity.
         * \param id The instance ID of the GameObject.
         * \param useGravity True to use gravity, false otherwise.
         */
        EXTERN_C UME_API inline void SetRigidbodyUseGravity(uint64_t id, bool useGravity)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.use_gravity = useGravity;
        }

        /*!
         * \brief Sets whether a GameObject's Rigidbody2D is kinematic.
         * \param id The instance ID of the GameObject.
         * \param isKinematic True to set as kinematic, false otherwise.
         */
        EXTERN_C UME_API inline void SetRigidbodyIsKinematic(uint64_t id, bool isKinematic)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& rb = go->GetComponent<Rigidbody2D>();
            rb.is_kinematic = isKinematic;
        }

        /*!
         * \brief Sets whether a GameObject's SpriteRender is flipped horizontally.
         * \param id The instance ID of the GameObject.
         * \param flipX True to flip horizontally, false otherwise.
         */
        EXTERN_C UME_API inline void SetSpriteRenderFlipX(uint64_t id, bool flipX)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& sr = go->GetComponent<SpriteRender>();
            sr.flipX = flipX;
        }

        /*!
         * \brief Sets whether a GameObject's SpriteRender is flipped vertically.
         * \param id The instance ID of the GameObject.
         * \param flipY True to flip vertically, false otherwise.
         */
        EXTERN_C UME_API inline void SetSpriteRenderFlipY(uint64_t id, bool flipY)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& sr = go->GetComponent<SpriteRender>();
            sr.flipY = flipY;
        }

        /*!
         * \brief Plays an animation on a GameObject.
         * \param id The instance ID of the GameObject.
         * \param clipName The name of the animation clip to play.
         * \return True if the animation was successfully set, false otherwise.
         */
        EXTERN_C UME_API inline bool PlayAnimation(uint64_t id, MonoString* clipName)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& anim = go->GetComponent<Animation>();
            return anim.SetAnimation(ScriptingEngine::MonoStringToUTF8(clipName));
        }

        /*!
         * \brief Plays an animation on a GameObject, queuing it if another animation is already playing.
         * \param id The instance ID of the GameObject.
         * \param clipName The name of the animation clip to play.
         * \return True if the animation was successfully set, false otherwise.
         */
        EXTERN_C UME_API inline bool PlayQueuedAnimation(uint64_t id, MonoString* clipName)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& anim = go->GetComponent<Animation>();
            return anim.SetAnimationUninterrupted(ScriptingEngine::MonoStringToUTF8(clipName));
        }

        /*!
         * \brief Plays an animation on a GameObject immediately, interrupting any currently playing animation.
         * \param id The instance ID of the GameObject.
         * \param clipName The name of the animation clip to play.
         * \return True if the animation was successfully set, false otherwise.
         */
        EXTERN_C UME_API inline bool PlayImmediately(uint64_t id, MonoString* clipName)
        {
            auto* go = GameObjectManager::GetInstance().GetGO(id);
            auto& anim = go->GetComponent<Animation>();
            return anim.SetAnimationImmediately(ScriptingEngine::MonoStringToUTF8(clipName));
        }

        /*!
         * \brief Gets the delta time for the current frame.
         * \return The delta time for the current frame.
         */
        EXTERN_C UME_API inline float GetDeltaTime()
        {
            return static_cast<float>(g_FrameRateController.GetDeltaTime());
        }

        /*!
         * \brief Gets the fixed delta time for the current frame.
         * \return The fixed delta time for the current frame.
         */
        EXTERN_C UME_API inline float GetFixedDeltaTime()
        {
            return static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
        }
    }
}
