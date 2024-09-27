/* Start Header
*****************************************************************/
/*!
\file       Collision.h
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Sept 24, 2024
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
    public:
        void CheckCollisions();
        void ResolveCollisions();

    public: //private:
        /*!***********************************************************************
        \brief
         Implementation of collision detection between two boxes.
        \param[in] box1
         The bounding box of the first box.
        \param[in] vel1
         The velocity of the first box.
        \param[in] box2
         The bounding box of the second box.
        \param[in] vel2
         The velocity of the second box.
        \param[out] firstTimeOfCollision
         Output variable for the calculated "tFirst" value.
        \return
         True if there is an intersection, false otherwise.
        *************************************************************************/
        static bool CollisionIntersection_BoxBox(const BoxCollider2D& box1,
            const Vec2& vel1,
            const BoxCollider2D& box2,
            const Vec2& vel2,
            float& firstTimeOfCollision);

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
        static bool CollisionIntersection_CircleCircle(const CircleCollider2D& circle1, const CircleCollider2D& circle2);

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
        static bool CollisionIntersection_CircleBox(const CircleCollider2D& circle, const BoxCollider2D& box);

        /*!***********************************************************************
        \brief
         Implementation of collision detection between a box and the screen boundaries.
        \param[in] box
        The box to collide.
        \param[in] screen_width
        The width of the game screen.
        \param[in] screen_height
        The height of the game screen.
        \return
         True if the box and screen boundary intersect, false otherwise.
        *************************************************************************/
        static bool CollisionIntersection_BoxScreen(const BoxCollider2D& box, const int screen_width, const int screen_height);

        void CollisionResponse_BoxBox();
        void CollisionResponse_CircleCircle();
        void CollisionResponse_CircleBox();
        void CollisionResponse_BoxScreen();
    };
}