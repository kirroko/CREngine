/* Start Header
*****************************************************************/
/*!
\file       Physics.cpp
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Oct 19, 2024
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
#include "../Math/MathUtils.h"  // for abs, clamp

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
     Add torque to the entity's rigidbody.
    \param[out] rb
     The rigidbody of the entity.
    \param[in] torque
     The amount of torque to add.
    *************************************************************************/
    void Physics::AddTorque(Rigidbody2D& rb, const float torque)
    {
        rb.torque = torque;
    }

    /*!***********************************************************************
    \brief
     Remove torque from the entity's rigidbody.
    \param[in/out] rb
     The rigidbody of the entity.
    *************************************************************************/
    void Physics::RemoveTorque(Rigidbody2D& rb)
    {
        rb.torque = 0.f;
        rb.angular_velocity *= rb.angular_drag;

        // Complete stop if almost still
        if (abs(rb.angular_velocity) < 0.01f)
            rb.angular_velocity = 0.f;
    }

    /*!***********************************************************************
    \brief
     Update the physics of all the entities.
    *************************************************************************/
    void Physics::UpdatePhysics()
    {
        // TEMP
        int current_num_of_steps = 1;

        // Update the physics based on the number of steps
        for (int step = 0; step < current_num_of_steps; ++step)
        {
            for (auto& entity : m_Entities)
            {
                // Get references of entity components
                auto& rb = ECS::GetInstance().GetComponent<Rigidbody2D>(entity);
                auto& trans = ECS::GetInstance().GetComponent<Transform>(entity);

                // Skip if entity is kinematic (static objects)
                if (rb.is_kinematic)
                    continue;

                // Update the linear physics of the entity
                UpdateLinearPhysics(rb, trans);

                // Update the rotational physics of the entity
                UpdateRotationalPhysics(rb, trans);
            }
        }
    }

    /*!***********************************************************************
    \brief
     Update the linear physics of all the entities.
    \param[in/out] rb
     The rigidbody of the entity.
    \param[in/out] trans
     The transform of the entity.
    *************************************************************************/
    void Physics::UpdateLinearPhysics(Rigidbody2D& rb, Transform& trans)
    {
        // TEMP
        const double fixedTimeStep = 1.0 / 60.0; // Fixed timestep for game logic

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
        //rb.velocity = rb.velocity + rb.acceleration * static_cast<float>(UME::g_FrameRateController.GetDeltaTime());
        rb.velocity = rb.velocity + rb.acceleration * static_cast<float>(fixedTimeStep); // Update based on fixed delta time

        // Clamp the velocity to prevent exceeding max velocity
        float current_vel = Vec2Length(rb.velocity);
        if (current_vel > MAX_VELOCITY)
        {
            // Normalize the velocity and scale it to the max velocity
            float scale = MAX_VELOCITY / current_vel;
            rb.velocity.x *= scale;
            rb.velocity.y *= scale;
        }

        // Apply velocity to the position (dx = v * dt)
        rb.position = rb.position + rb.velocity * static_cast<float>(fixedTimeStep); // Update based on fixed delta time

        // Update transform position with the physics position
        trans.position = rb.position;
    }

    /*!***********************************************************************
    \brief
     Update the rotational physics of all the entities.
    \param[in/out] rb
     The rigidbody of the entity.
    \param[in/out] trans
     The transform of the entity.
    *************************************************************************/
    void Physics::UpdateRotationalPhysics(Rigidbody2D& rb, Transform& trans)
    {
        // TEMP
        const double fixedTimeStep = 1.0 / 60.0; // Fixed timestep for game logic

        // Update physics angle with the transform rotation
        rb.angle = trans.rotation;

        // Apply torque and inertia mass to the angular acceleration
        rb.angular_acceleration = rb.torque * rb.inv_inertia_mass;

        // Apply angular acceleration to the angular velocity
        rb.angular_velocity = rb.angular_velocity + rb.angular_acceleration * static_cast<float>(fixedTimeStep);

        // Clamp the angular velocity to prevent exceeding max angular velocity
        rb.angular_velocity = clamp(rb.angular_velocity, -MAX_VELOCITY, MAX_VELOCITY);

        // Apply angular velocity to the angle
        rb.angle = rb.angle + rb.angular_velocity * static_cast<float>(fixedTimeStep);

        // Update transform rotation with the physics angle
        trans.rotation = rb.angle;
    }
}