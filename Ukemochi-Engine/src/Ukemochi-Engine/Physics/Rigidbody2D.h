/* Start Header
*****************************************************************/
/*!
\file       Rigidbody2D.h
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Sept 24, 2024
\brief      This file contains the declaration of the Rigidbody2D class.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#include "../Math/Vector2D.h" // for Vector2D
#include "../Math/Matrix3x3.h" // for Matrix3x3
#include "../Collision/BoxCollider2D.h" // for BoxCollider2D struct

namespace Ukemochi
{
    class Rigidbody2D
    {
    public: // private:
        Mtx33 transform{};
        Vec2 position_curr{};
        Vec2 position_prev{};
        Vec2 velocity{};
        Vec2 acceleration{ 7.f, 7.f };
        Vec2 scale{ 100.f, 100.f };

        float direction{};
        float angle{};
        float angular_velocity{};
        float angular_acceleration{};

        Vec2 initial_position{};
        Ukemochi::BoxCollider2D box_collider;
        bool active{ true };

    public:
        //Rigidbody2D() : transform(Mtx33{}), position(Vec2{}), velocity(Vec2{}), movement_speed(1.0f), direction(0.0f) {}
        //Rigidbody2D(Mtx33 trans, Vec2 pos, Vec2 vel, float speed, float dir) : transform(trans), position(pos), velocity(vel), movement_speed(speed), direction(dir) {}
    };
}