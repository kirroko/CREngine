/* Start Header
*****************************************************************/
/*!
\file       Physics.cpp
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Oct 11, 2024
\brief      This file contains the definition of the Physics system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "PreCompile.h"
#include "Physics.h"            // for forward declaration
#include "../FrameController.h" // for GetDeltaTime
#include "../ECS/ECS.h"         // for entity components
#include "../Math/MathUtils.h"  // for abs

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
    void Physics::AddForceX(Rigidbody2D& rb, const float force)
    {
        rb.force.x = force;
    }

    /*!***********************************************************************
    \brief
     Add force to the entity's rigidbody in the y-axis.
    \param[out] rb
     The rigidbody of the entity.
    \param[in] force
     The amount of force to add.
    *************************************************************************/
    void Physics::AddForceY(Rigidbody2D& rb, const float force)
    {
        rb.force.y = force;
    }

    /*!***********************************************************************
    \brief
     Remove force from the entity's rigidbody in the x-axis.
    \param[in/out] rb
     The rigidbody of the entity.
    *************************************************************************/
    void Physics::RemoveForceX(Rigidbody2D& rb)
    {
        rb.force.x = 0.f;
        rb.velocity.x *= rb.linear_drag;

        // Complete stop if almost still
        if (abs(rb.velocity.x) < 0.01f)
            rb.velocity.x = 0.f;
    }

    /*!***********************************************************************
    \brief
     Remove force from the entity's rigidbody in the y-axis.
    \param[in/out] rb
     The rigidbody of the entity.
    *************************************************************************/
    void Physics::RemoveForceY(Rigidbody2D& rb)
    {
        rb.force.y = 0.f;
        rb.velocity.y *= rb.linear_drag;

        // Complete stop if almost still
        if (abs(rb.velocity.y) < 0.01f)
            rb.velocity.y = 0.f;
    }

    /*!***********************************************************************
    \brief
     Update the physics of all the entities.
    *************************************************************************/
    void Physics::UpdatePhysics()
    {
        for (auto& entity : m_Entities)
        {
            // Get references of entity components
            auto& trans = ECS::GetInstance().GetComponent<Transform>(entity);
            auto& box = ECS::GetInstance().GetComponent<BoxCollider2D>(entity);
            auto& rb = ECS::GetInstance().GetComponent<Rigidbody2D>(entity);

            // Get the bounding box size
            box.min = { -Ukemochi::BOUNDING_BOX_SIZE * trans.scale.x + trans.position.x,
                        -Ukemochi::BOUNDING_BOX_SIZE * trans.scale.y + trans.position.y };
            box.max = { Ukemochi::BOUNDING_BOX_SIZE * trans.scale.x + trans.position.x,
                        Ukemochi::BOUNDING_BOX_SIZE * trans.scale.y + trans.position.y };

            // Skip if entity is kinematic (static objects)
            if (rb.is_kinematic)
                continue;

            // Update physics position with the transform position
            rb.position = trans.position;

            // Normalize force for diagonal movement
            if (rb.force.x != 0 && rb.force.y != 0) {
                rb.force.x *= NORMALIZATION_FACTOR;
                rb.force.y *= NORMALIZATION_FACTOR;
            }

            // Apply force, mass and gravity to the acceleration (a = F / m)
            rb.acceleration = rb.force * rb.inverse_mass;
            /*if (rb.use_gravity)
                rb.acceleration = rb.force * rb.inverse_mass + GRAVITY;
            else*/

            // Apply acceleration to the velocity (dv = a * dt)
            rb.velocity = rb.velocity + rb.acceleration * static_cast<float>(UME::g_FrameRateController.GetDeltaTime());

            // Clamp the velocity to prevent exceeding max velocity
            float current_vel = Vec2Length(rb.velocity);

            // If the current velocity exceeds the max velocity, scale the velocity down
            if (current_vel > MAX_VELOCITY)
            {
                // Normalize the velocity and scale it to the max velocity
                float scale = MAX_VELOCITY / current_vel;
                rb.velocity.x *= scale;
                rb.velocity.y *= scale;
            }

            // Apply velocity to the position (dx = v * dt)
            rb.position = rb.position + rb.velocity * static_cast<float>(UME::g_FrameRateController.GetDeltaTime());

            // Update transform position with the physics position
            trans.position = rb.position;

            // Apply torque and inertia mass to the angular acceleration
            // Apply angular acceleration to the angular velocity
            // Apply angular velocity to the angle
            // Update transform rotation with the physics angle
        }
    }
}