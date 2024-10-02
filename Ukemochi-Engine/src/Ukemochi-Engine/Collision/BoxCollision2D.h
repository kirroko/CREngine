/* Start Header
*****************************************************************/
/*!
\file       BoxCollision2D.h
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Sept 11, 2024
\brief      This file contains the declaration of the BoxCollision2D system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#include "../Math/Vector2D.h" // for Math::Vec2

namespace Ukemochi
{
    /*!***********************************************************************
    \brief
     Axis-Aligned Bounding Box (AABB) structure.
    *************************************************************************/
    struct AABB
    {
        Vec2 min;
        Vec2 max;
    };

    /*!***********************************************************************
    \brief
     Normalized bounding rectangle (width and height) sizes - AABB collision data.
    *************************************************************************/
    const float BOUNDING_RECT_SIZE = 0.5f;

    /*!***********************************************************************
    \brief
     Implementation of collision detection between two rectangles.
    \param[in] aabb1
     The bounding box of the first rectangle.
    \param[in] vel1
     The velocity of the first rectangle.
    \param[in] aabb2
     The bounding box of the second rectangle.
    \param[in] vel2
     The velocity of the second rectangle.
    \param[out] firstTimeOfCollision
     Output variable for the calculated "tFirst" value.
    \return
     True if there is an intersection, false otherwise.
    *************************************************************************/
    bool CollisionIntersection_RectRect(const AABB &aabb1,
                                        const Vec2 &vel1,
                                        const AABB &aabb2,
                                        const Vec2 &vel2,
                                        float &firstTimeOfCollision);
}