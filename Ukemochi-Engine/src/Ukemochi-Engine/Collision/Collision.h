/* Start Header
*****************************************************************/
/*!
\file       Collision.h
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Sept 11, 2024
\brief      This file contains the declaration of the Collision system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#include "BoxCollision2D.h"    // for Box collision functions
#include "CircleCollision2D.h" // for Circle collision functions

namespace Ukemochi
{
    void CheckCollisions();
    void ResolveCollisions();
}