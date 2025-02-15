/* Start Header ************************************************************************/
/*!
\file       Physics.h
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Feb 15, 2025
\brief      This file contains the declaration of the Physics system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include "../ECS/ECS.h" // for ECS system and components

namespace Ukemochi
{
    class Physics : public System
    {
    private:
        const Vec2 GRAVITY{ 0.0f, 9.8f };                          // Gravity constant to simulate gravity for physics.

        const float PLAYER_MAX_VELOCITY = 600.0f;                  // Maximum linear velocity constant for the player.
        const float ENEMY_MAX_VELOCITY = 300.0f;                   // Maximum linear velocity constant for the enemy.
        const float MAX_ANGULAR_VELOCITY = 300.0f;                 // Maximum angular velocity constant for all entities.

        const float NORMALIZATION_FACTOR = 1.0f / std::sqrt(2.0f); // Normalize factor for diagonal movement, ~0.7071

    public:
        /*!***********************************************************************
        \brief
         Add force to the entity's rigidbody in the x-axis.
        \param[out] rb
         The rigidbody of the entity.
        \param[in] force
         The amount of force to add.
        *************************************************************************/
        void AddForceX(Rigidbody2D& rb, const float force);

        /*!***********************************************************************
        \brief
         Add force to the entity's rigidbody in the y-axis.
        \param[out] rb
         The rigidbody of the entity.
        \param[in] force
         The amount of force to add.
        *************************************************************************/
        void AddForceY(Rigidbody2D& rb, const float force);

        /*!***********************************************************************
        \brief
         Remove force from the entity's rigidbody in the x-axis.
        \param[out] rb
         The rigidbody of the entity.
        *************************************************************************/
        void RemoveForceX(Rigidbody2D& rb);

        /*!***********************************************************************
        \brief
         Remove force from the entity's rigidbody in the y-axis.
        \param[out] rb
         The rigidbody of the entity.
        *************************************************************************/
        void RemoveForceY(Rigidbody2D& rb);

        /*!***********************************************************************
        \brief
         Add torque to the entity's rigidbody.
        \param[out] rb
         The rigidbody of the entity.
        \param[in] torque
         The amount of torque to add.
        *************************************************************************/
        void AddTorque(Rigidbody2D& rb, const float torque);

        /*!***********************************************************************
        \brief
         Remove torque from the entity's rigidbody.
        \param[out] rb
         The rigidbody of the entity.
        *************************************************************************/
        void RemoveTorque(Rigidbody2D& rb);

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
        void ApplyKnockback(const Transform& source_trans, const float source_force, const Transform& target_trans, Rigidbody2D& target_rb);

        /*!***********************************************************************
        \brief
         Update the physics of all the entities.
        *************************************************************************/
        void UpdatePhysics();

    private:
        /*!***********************************************************************
        \brief
         Update the linear physics of all the entities.
        \param[in] tag
         The tag of the entity.
        \param[in/out] trans
         The transform of the entity.
        \param[in/out] rb
         The rigidbody of the entity.
        *************************************************************************/
        void UpdateLinearPhysics(const std::string& tag, Transform& trans, Rigidbody2D& rb);

        /*!***********************************************************************
        \brief
         Update the rotational physics of all the entities.
        \param[in] tag
         The tag of the entity.
        \param[in/out] trans
         The transform of the entity.
        \param[in/out] rb
         The rigidbody of the entity.
        *************************************************************************/
        void UpdateRotationalPhysics(const std::string& tag, Transform& trans, Rigidbody2D& rb);
    };
}