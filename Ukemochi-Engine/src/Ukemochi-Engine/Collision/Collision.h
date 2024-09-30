/* Start Header
*****************************************************************/
/*!
\file       Collision.h
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Sept 29, 2024
\brief      This file contains the declaration of the Collision system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#include "../ECS/Components.h" // for CircleCollider2D, BoxCollider2D components
#include "../ECS/Systems.h"    // for System inheritance

namespace Ukemochi
{
    class Collision : public System
    {
    private:
        // Collision flags
        const unsigned int COLLISION_LEFT = 0x00000001;   //0001
        const unsigned int COLLISION_RIGHT = 0x00000002;  //0010
        const unsigned int COLLISION_TOP = 0x00000004;    //0100
        const unsigned int COLLISION_BOTTOM = 0x00000008; //1000

        const float MIN_OFFSET = 0.001f;

        int screen_width, screen_height;

    public:
        /*!***********************************************************************
        \brief
         Check the collisions of all the entities.
        *************************************************************************/
        void CheckCollisions();

    private:
        /*!***********************************************************************
        \brief
         Implementation of collision detection between two boxes.
        \param[in/out] box1
         The bounding box of the first box.
        \param[in] vel1
         The velocity of the first box.
        \param[in/out] box2
         The bounding box of the second box.
        \param[in] vel2
         The velocity of the second box.
        \param[out] firstTimeOfCollision
         Output variable for the calculated "tFirst" value.
        \return
         True if there is an intersection, false otherwise.
        *************************************************************************/
        bool CollisionIntersection_BoxBox(BoxCollider2D& box1, const Vec2& vel1, BoxCollider2D& box2, const Vec2& vel2, float& firstTimeOfCollision);

        /*!***********************************************************************
        \brief
         Collision response for collision between two boxes.
        \param[in] box1
         The first box that collided.
        \param[in] box2
         The second box that collided.
        \param[out] trans1
         The transform component attached to the first collided object.
        \param[out] trans2
         The transform component attached to the second collided object.
        \param[out] rb1
         The rigidbody component attached to the first collided object.
        \param[out] rb2
         The rigidbody component attached to the second collided object.
        *************************************************************************/
        void CollisionResponse_BoxBox(const BoxCollider2D& box1, const BoxCollider2D& box2, Transform& trans1, Transform& trans2, Rigidbody2D& rb1, Rigidbody2D& rb2);

        /*!***********************************************************************
        \brief
         Implementation of collision detection between a box and the screen boundaries.
        \param[in/out] box
         The box to collide.
        \param[in] screen_width
         The width of the game screen.
        \param[in] screen_height
         The height of the game screen.
        \return
         Collision flags if the box and screen boundary intersect, no flag otherwise.
        *************************************************************************/
        int CollisionIntersection_BoxScreen(BoxCollider2D& box, const int screen_width, const int screen_height);

        /*!***********************************************************************
        \brief
         Collision response for collision between a box and the screen boundaries.
        \param[in] box
         The box that collided.
        \param[in] screen_width
         The width of the game screen.
        \param[in] screen_height
         The height of the game screen.
        \param[out] trans
         The transform component attached to the collided object.
        \param[out] rb
         The rigidbody component attached to the collided object.
        *************************************************************************/
        void CollisionResponse_BoxScreen(const BoxCollider2D& box, const int screen_width, const int screen_height, Transform& trans, Rigidbody2D& rb);

        /*!***********************************************************************
        \brief
         Implementation of collision detection between two circles.
        \param[in] circle1
         The first circle.
        \param[in] circle2
         The second circle.
        \return
         True if the circles intersect, false otherwise.
        *************************************************************************/
        bool CollisionIntersection_CircleCircle(const CircleCollider2D& circle1, const CircleCollider2D& circle2);

        /*!***********************************************************************
        \brief
         Implementation of collision detection between a circle and a box.
        \param[in] circle
         The circle to collide.
        \param[in] box
         The box to collide.
        \return
         True if the circle and box intersect, false otherwise.
        *************************************************************************/
        bool CollisionIntersection_CircleBox(const CircleCollider2D& circle, const BoxCollider2D& box);
    };
}