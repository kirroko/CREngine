/* Start Header ************************************************************************/
/*!
\file       Physics.cpp
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Nov 17, 2024
\brief      This file contains the definition of the Physics system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "Physics.h"                      // for forward declaration
#include "../FrameController.h"           // for GetCurrentNumberOfSteps, GetFixedDeltaTime
#include "../Math/MathUtils.h"            // for abs, clamp
#include "../Factory/GameObjectManager.h" // for game object tag

namespace Ukemochi
{
    /*!***********************************************************************
    \brief
     Add force to the entity's rigidbody in the x-axis.
    \param[out] rb
     The rigidbody of the entity.
    \param[in] force
     The amount of force to add.
    *************************************************************************/
    void Physics::AddForceX(Rigidbody2D& rb, const float force) { rb.force.x = force; }

    /*!***********************************************************************
    \brief
     Add force to the entity's rigidbody in the y-axis.
    \param[out] rb
     The rigidbody of the entity.
    \param[in] force
     The amount of force to add.
    *************************************************************************/
    void Physics::AddForceY(Rigidbody2D& rb, const float force) { rb.force.y = force; }

    /*!***********************************************************************
    \brief
     Remove force from the entity's rigidbody in the x-axis.
    \param[in/out] rb
     The rigidbody of the entity.
    *************************************************************************/
    void Physics::RemoveForceX(Rigidbody2D& rb) { rb.force.x = 0.f; }

    /*!***********************************************************************
    \brief
     Remove force from the entity's rigidbody in the y-axis.
    \param[in/out] rb
     The rigidbody of the entity.
    *************************************************************************/
    void Physics::RemoveForceY(Rigidbody2D& rb) { rb.force.y = 0.f; }

    /*!***********************************************************************
    \brief
     Add torque to the entity's rigidbody.
    \param[out] rb
     The rigidbody of the entity.
    \param[in] torque
     The amount of torque to add.
    *************************************************************************/
    void Physics::AddTorque(Rigidbody2D& rb, const float torque) { rb.torque = torque; }

    /*!***********************************************************************
    \brief
     Remove torque from the entity's rigidbody.
    \param[in/out] rb
     The rigidbody of the entity.
    *************************************************************************/
    void Physics::RemoveTorque(Rigidbody2D& rb) { rb.torque = 0.f; }

    /*!***********************************************************************
    \brief
     Apply a knockback effect to a target based on the direction and force of the source.
    \param[in] source_trans
     The transform of the source entity.
    \param[in] source_force
     The amount of force to apply to the target entity.
    \param[in] target_trans
     The transform of the target entity.
    \param[out] target_rb
     The rigidbody of the target entity.
    *************************************************************************/
    void Physics::ApplyKnockback(const Transform& source_trans, const float source_force, const Transform& target_trans, Rigidbody2D& target_rb)
    {
        Vec2 knockback_direction{};

        // Get the knockback direction
        Vec3 new_value = target_trans.position - source_trans.position;
        Vec2Normalize(knockback_direction, Vec2(new_value.x, new_value.y));
        knockback_direction *= source_force;

        // Apply the knockback force
        AddForceX(target_rb, knockback_direction.x);
        AddForceY(target_rb, knockback_direction.y);
    }

    /*!***********************************************************************
    \brief
     Update the physics of all the entities.
    *************************************************************************/
    void Physics::UpdatePhysics()
    {
        // Update the physics based on the number of steps
        for (int step = 0; step < g_FrameRateController.GetCurrentNumberOfSteps(); ++step)
        {
            for (auto& entity : m_Entities)
            {
                // Skip if the entity is not active
                if (!GameObjectManager::GetInstance().GetGO(entity)->GetActive())
                    continue;

                // Get references of entity components
                auto& trans = ECS::GetInstance().GetComponent<Transform>(entity);
                auto& rb = ECS::GetInstance().GetComponent<Rigidbody2D>(entity);

                // Skip if entity is kinematic (static objects)
                if (rb.is_kinematic)
                    continue;

                // Get the tag of the entity
                std::string tag = GameObjectManager::GetInstance().GetGO(entity)->GetTag();

                // Update the linear physics of the entity
                UpdateLinearPhysics(trans, rb);

                // Update the rotational physics of the entity
                UpdateRotationalPhysics(trans, rb);
            }
        }
    }

    /*!***********************************************************************
    \brief
     Update the linear physics of all the entities.
    \param[in/out] trans
     The transform of the entity.
    \param[in/out] rb
     The rigidbody of the entity.
    *************************************************************************/
    void Physics::UpdateLinearPhysics(Transform& trans, Rigidbody2D& rb)
    {
        // Update physics position with the transform position
        rb.position = trans.position;

        // Normalize force for diagonal movement
        if (rb.force.x != 0 && rb.force.y != 0)
        {
            rb.force.x *= NORMALIZATION_FACTOR;
            rb.force.y *= NORMALIZATION_FACTOR;
        }

        // Apply force, mass and gravity to the acceleration (a = F / m)
        if (rb.use_gravity)
            rb.acceleration = rb.force * rb.inverse_mass + GRAVITY;
        else
            rb.acceleration = rb.force * rb.inverse_mass;

        // Apply acceleration to the velocity (dv = a * dt)
        rb.velocity = rb.velocity + rb.acceleration * static_cast<float>(g_FrameRateController.GetFixedDeltaTime());

        // Clamp the velocity to prevent exceeding max velocity
        float current_vel = Vec2Length(rb.velocity);
        if (current_vel > MAX_VELOCITY)
        {
            // Normalize the velocity and scale it to the max velocity
            float scale = MAX_VELOCITY / current_vel;
            rb.velocity.x *= scale;
            rb.velocity.y *= scale;
        }

        // Apply linear drag to the velocity of the entity
        rb.velocity.x *= rb.linear_drag;
        if (abs(rb.velocity.x) < 0.01f)
            rb.velocity.x = 0.f; // Complete stop if almost still

        rb.velocity.y *= rb.linear_drag;
        if (abs(rb.velocity.y) < 0.01f)
            rb.velocity.y = 0.f; // Complete stop if almost still

        // Apply velocity to the position (dx = v * dt)
        Vec2 new_value = rb.velocity * static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
        rb.position = rb.position + Vec3(new_value.x, new_value.y, 0.f);

        // Update transform position with the physics position
        trans.position = rb.position;
    }

    /*!***********************************************************************
    \brief
     Update the rotational physics of all the entities.
    \param[in/out] trans
     The transform of the entity.
    \param[in/out] rb
     The rigidbody of the entity.
    *************************************************************************/
    void Physics::UpdateRotationalPhysics(Transform& trans, Rigidbody2D& rb)
    {
        // Update physics angle with the transform rotation
        rb.angle = trans.rotation;

        // Apply torque and inertia mass to the angular acceleration
        rb.angular_acceleration = rb.torque * rb.inv_inertia_mass;

        // Apply angular acceleration to the angular velocity
        rb.angular_velocity = rb.angular_velocity + rb.angular_acceleration * static_cast<float>(g_FrameRateController.GetFixedDeltaTime());

        // Clamp the angular velocity to prevent exceeding max angular velocity
        rb.angular_velocity = clamp(rb.angular_velocity, -MAX_VELOCITY, MAX_VELOCITY);

        // Apply angular drag to the angular velocity of the entity
        rb.angular_velocity *= rb.angular_drag;
        if (abs(rb.angular_velocity) < 0.01f)
            rb.angular_velocity = 0.f; // Complete stop if almost still

        // Apply angular velocity to the angle
        rb.angle = rb.angle + rb.angular_velocity * static_cast<float>(g_FrameRateController.GetFixedDeltaTime());

        // Update transform rotation with the physics angle
        trans.rotation = rb.angle;
    }
}