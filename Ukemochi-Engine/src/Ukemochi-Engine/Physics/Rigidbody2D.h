/* Start Header
*****************************************************************/
/*!
\file       Rigidbody2D.h
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Sept 24, 2024
\brief      This file contains the declaration of the Rigidbody2D component.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#include "../Math/Vector2D.h" // for Vector2D
#include "../Math/Matrix3x3.h" // for Matrix3x3
#include "../Collision/BoxCollider2D.h" // for BoxCollider2D component

namespace Ukemochi
{
    /*!***********************************************************************
    \brief
     Rigidbody2D component structure.
    *************************************************************************/
    struct Rigidbody2D
    {
        Vec2 position_curr{};
        Vec2 position_prev{};
        Vec2 velocity{};
        Vec2 acceleration{ 7.f, 7.f };
        Vec2 scale{ 100.f, 100.f };

        float direction{};
        float angle{};
        float angular_velocity{};
        float angular_acceleration{};

        // temp
        Mtx33 transform{};
        Vec2 initial_position{};
        bool active{ true };
    };
}