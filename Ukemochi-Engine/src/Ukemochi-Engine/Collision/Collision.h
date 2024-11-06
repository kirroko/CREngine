/* Start Header
*****************************************************************/
/*!
\file       Collision.h
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Nov 6, 2024
\brief      This file contains the declaration of the Collision system.

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
    class Collision : public System
    {
    private:
        /*!***********************************************************************
        \brief
         Normalized bounding box (width and height) sizes.
        *************************************************************************/
        const float BOUNDING_BOX_SIZE = 0.5f;

        // Collision flags
        const unsigned int COLLISION_LEFT = 0x00000001;   //0001
        const unsigned int COLLISION_RIGHT = 0x00000002;  //0010
        const unsigned int COLLISION_TOP = 0x00000004;    //0100
        const unsigned int COLLISION_BOTTOM = 0x00000008; //1000

        const float MIN_OFFSET = 0.001f;

        int screen_width{}, screen_height{};

    public:
        /*!***********************************************************************
        \brief
         Initialize the collision system.
        *************************************************************************/
        void Init();

        /*!***********************************************************************
        \brief
         Check the collisions of all the objects.
        *************************************************************************/
        void CheckCollisions();

    private:
        /*!***********************************************************************
        \brief
         Update the bounding box of the object.
        \param[out] box
         The BoxCollider2D component attached to the object.
        \param[in] trans
         The Transform component attached to the object.
        *************************************************************************/
        void UpdateBoundingBox(BoxCollider2D& box, const Transform& trans);

        /*!***********************************************************************
        \brief
         Collision detection between two boxes.
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
        bool BoxBox_Intersection(BoxCollider2D& box1, const Vec2& vel1, BoxCollider2D& box2, const Vec2& vel2, float& firstTimeOfCollision);

        /*!***********************************************************************
        \brief
         Collision detection between a box and the screen boundaries.
        \param[in/out] box
         The box to collide.
        \return
         Collision flags if the box and screen boundary intersect, no flag otherwise.
        *************************************************************************/
        int BoxScreen_Intersection(BoxCollider2D& box);

        /*!***********************************************************************
        \brief
         Collision detection between two circles.
        \param[in] circle1
         The first circle.
        \param[in] circle2
         The second circle.
        \return
         True if the circles intersect, false otherwise.
        *************************************************************************/
        bool CircleCircle_Intersection(const CircleCollider2D& circle1, const CircleCollider2D& circle2);

        /*!***********************************************************************
        \brief
         Collision detection between a circle and a box.
        \param[in] circle
         The circle to collide.
        \param[in] box
         The box to collide.
        \return
         True if the circle and box intersect, false otherwise.
        *************************************************************************/
        bool CircleBox_Intersection(const CircleCollider2D& circle, const BoxCollider2D& box);

        /*!***********************************************************************
        \brief
         Collision response between two objects.
        \param[out] trans1
         The Transform component attached to the first collided object.
        \param[in] box1
         The BoxCollider2D component attached to the first collided object.
        \param[out] rb1
         The Rigidbody2D component attached to the first collided object.
        \param[out] trans2
         The Transform component attached to the second collided object.
        \param[in] box2
         The BoxCollider2D component attached to the second collided object.
        \param[out] rb2
         The Rigidbody2D component attached to the second collided object.
        \param[in] firstTimeOfCollision
         The calculated first time of collision.
        *************************************************************************/
        void BoxBox_Response(Transform& trans1, BoxCollider2D& box1, Rigidbody2D& rb1, Transform& trans2, BoxCollider2D& box2, Rigidbody2D& rb2, float firstTimeOfCollision);

        /*!***********************************************************************
        \brief
         Collision response between a static object and a dynamic object, and between two dynamic objects.
        \param[out] trans1
         The Transform component attached to the first collided object.
        \param[in] box1
         The BoxCollider2D component attached to the first collided object.
        \param[out] rb1
         The Rigidbody2D component attached to the first collided object.
        \param[out] trans2
         The Transform component attached to the second collided object.
        \param[in] box2
         The BoxCollider2D component attached to the second collided object.
        \param[out] rb2
         The Rigidbody2D component attached to the second collided object.
        \param[in] firstTimeOfCollision
         The calculated first time of collision.
        *************************************************************************/
        void StaticDynamic_Response(Transform& trans1, BoxCollider2D& box1, Rigidbody2D& rb1, Transform& trans2, BoxCollider2D& box2, Rigidbody2D& rb2, float firstTimeOfCollision);

        /*!***********************************************************************
        \brief
         Collision response between a static object and a dynamic object.
        \param[out] trans1
         The Transform component attached to the first collided object.
        \param[in] box1
         The BoxCollider2D component attached to the first collided object.
        \param[in] rb1
         The Rigidbody2D component attached to the first collided object.
        \param[out] trans2
         The Transform component attached to the second collided object.
        \param[in] box2
         The BoxCollider2D component attached to the second collided object.
        \param[in] rb2
         The Rigidbody2D component attached to the second collided object.
        *************************************************************************/
        void Static_Response(Transform& trans1, const BoxCollider2D& box1, const Rigidbody2D& rb1, Transform& trans2, const BoxCollider2D& box2, const Rigidbody2D& rb2);

        /*!***********************************************************************
        \brief
         Collision response between two dynamic objects.
        \param[out] trans1
         The Transform component attached to the first collided object.
        \param[out] rb1
         The Rigidbody2D component attached to the first collided object.
        \param[out] trans2
         The Transform component attached to the second collided object.
        \param[out] rb2
         The Rigidbody2D component attached to the second collided object.
        *************************************************************************/
        void Dynamic_Response(Transform& trans1, Rigidbody2D& rb1, Transform& trans2, Rigidbody2D& rb2);

        /*!***********************************************************************
        \brief
         Collision response between the player and a trigger object.
        \param[out] player_trans
         The Transform component attached to the player object.
        \param[in] trigger_trans
         The Transform component attached to the trigger object.
        \param[in] trigger_box
         The BoxCollider2D component attached to the trigger object.
        *************************************************************************/
        void Trigger_Response(Transform& player_trans, const Transform& trigger_trans, const BoxCollider2D& trigger_box);

        /*!***********************************************************************
        \brief
         Collision response between an object and the screen boundaries.
        \param[out] trans
         The Transform component attached to the collided object.
        \param[in] box
         The BoxCollider2D component attached to the collided object.
        \param[out] rb
         The Rigidbody2D component attached to the collided object.
        *************************************************************************/
        void BoxScreen_Response(Transform& trans, const BoxCollider2D& box, Rigidbody2D& rb);
    };
}