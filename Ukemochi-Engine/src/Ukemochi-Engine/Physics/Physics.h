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

#include "../ECS/Components.h" // for Rigidbody2D, BoxCollider2D components
#include "../ECS/Systems.h"    // for System inheritance

namespace Ukemochi
{
    const float GRAVITY = -9.8f;

    /*!***********************************************************************
    \brief
     Normalized bounding box (width and height) sizes - BoxCollider2D collision data.
    *************************************************************************/
    const float BOUNDING_BOX_SIZE = 0.5f;

    class Physics : public System
    {
    public:
        void UpdatePhysics();
        void UpdatePhysics(EntityID entity);
    };
}