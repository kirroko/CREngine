/* Start Header
*****************************************************************/
/*!
\file       Physics.h
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Oct 31, 2024
\brief      This file contains the declaration of the Physics system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#include "../ECS/ECS.h" // for ECS system and components

namespace Ukemochi
{
    /*!***********************************************************************
    \brief
     Gravity constant to simulate gravity for physics.
    *************************************************************************/
    const Vec2 GRAVITY{ 0.0f, 9.8f };

    /*!***********************************************************************
    \brief
     Maximum velocity constant for all entities.
    *************************************************************************/
    const float MAX_VELOCITY = 300.0f; // Adjust this value for your desired max velocity

    /*!***********************************************************************
    \brief
     Normalize factor for diagonal movement.
    *************************************************************************/
    const float NORMALIZATION_FACTOR = 1.0f / std::sqrt(2.0f); // = ~0.7071

    class Physics : public System
    {
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
        \param[in/out] rb
         The rigidbody of the entity.
        *************************************************************************/
        void RemoveForceX(Rigidbody2D& rb);

        /*!***********************************************************************
        \brief
         Remove force from the entity's rigidbody in the y-axis.
        \param[in/out] rb
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
        \param[in/out] rb
         The rigidbody of the entity.
        *************************************************************************/
        void RemoveTorque(Rigidbody2D& rb);

        /*!***********************************************************************
        \brief
         Update the physics of all the entities.
        *************************************************************************/
        void UpdatePhysics();

    private:
        /*!***********************************************************************
        \brief
         Update the linear physics of all the entities.
        \param[in/out] rb
         The rigidbody of the entity.
        \param[in/out] trans
         The transform of the entity.
        *************************************************************************/
        void UpdateLinearPhysics(Rigidbody2D& rb, Transform& trans);

        /*!***********************************************************************
        \brief
         Update the rotational physics of all the entities.
        \param[in/out] rb
         The rigidbody of the entity.
        \param[in/out] trans
         The transform of the entity.
        *************************************************************************/
        void UpdateRotationalPhysics(Rigidbody2D& rb, Transform& trans);
    };
}