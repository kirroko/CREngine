/* Start Header
*****************************************************************/
/*!
\file       Physics.h
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Sept 25, 2024
\brief      This file contains the declaration of the Physics system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#include "Rigidbody2D.h"                // for Rigidbody2D class
#include "../Collision/BoxCollider2D.h" // for BoxCollider2D struct

namespace Ukemochi
{
    const float GRAVITY = -9.8f;

    class Physics
    {
    public:
        static void UpdatePhysics(Rigidbody2D& rb, BoxCollider2D& box);
    };
}