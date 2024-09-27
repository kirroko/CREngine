/* Start Header
*****************************************************************/
/*!
\file       BoxCollider2D.h
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Sept 24, 2024
\brief      This file contains the declaration of the BoxCollider2D component.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#include "../Math/Vector2D.h" // for Vec2

namespace Ukemochi
{
    /*!***********************************************************************
    \brief
     BoxCollider2D (Axis-Aligned Bounding Box) component structure.
    *************************************************************************/
    struct BoxCollider2D
    {
        Vec2 min;
        Vec2 max;
    };
}