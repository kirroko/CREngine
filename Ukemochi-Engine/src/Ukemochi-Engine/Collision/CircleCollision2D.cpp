/* Start Header
*****************************************************************/
/*!
\file       CircleCollision2D.cpp
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

#include "PreCompile.h"
#include "CircleCollision2D.h"
#include "../Math/MathUtils.h" // for Math::abs

namespace Ukemochi
{
    /*!***********************************************************************
    \brief
     Implementation of collision detection between two circles.
    *************************************************************************/
    bool CollisionIntersection_CircleCircle(const Circle &circle1, const Circle &circle2)
    {
        float dx{circle2.m_center.x - circle1.m_center.x};
        float dy{circle2.m_center.y - circle1.m_center.y};
        float distance_squared{dx * dx + dy * dy};

        return distance_squared <= (circle1.m_radius + circle2.m_radius) * (circle1.m_radius + circle2.m_radius);
    }

    bool CollisionIntersection_CircleAABB(const Circle &circle, const AABB &aabb)
    {
        float dx = abs(circle.m_center.x - (aabb.min.x + aabb.max.x) / 2);
        float dy = abs(circle.m_center.y - (aabb.min.y + aabb.max.y) / 2);

        // Check if circle center is inside the AABB
        if (dx <= (aabb.max.x - aabb.min.x) / 2 && dy <= (aabb.max.y - aabb.min.y) / 2)
            return true;

        // Check if circle is intersecting any of the AABB's edges
        double distanceSquared = dx * dx + dy * dy;
        double radiusSquared = circle.m_radius * circle.m_radius;
        return distanceSquared <= radiusSquared;
    }
}