/* Start Header
*****************************************************************/
/*!
\file       CircleCollision2D.h
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Sept 11, 2024
\brief      This file contains the declaration of the CircleCollision2D system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#include "../Math/Vector2D.h" // for Vec2
#include "BoxCollision2D.h" // for AABB

namespace Ukemochi
{
    /*!***********************************************************************
    \brief
     Represent a circle structure.
    *************************************************************************/
    struct Circle
    {
        Vec2 m_center;
        float m_radius;
    };

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
    bool CollisionIntersection_CircleCircle(const Circle &circle1, const Circle &circle2);

    bool CollisionIntersection_CircleAABB(const Circle &circle, const AABB &aabb);
}