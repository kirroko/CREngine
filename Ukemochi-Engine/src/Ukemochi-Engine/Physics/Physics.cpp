/* Start Header
*****************************************************************/
/*!
\file       Physics.cpp
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Sept 29, 2024
\brief      This file contains the definition of the Physics system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "PreCompile.h"
#include "Physics.h"            // for forward declaration
#include "../FrameController.h" // for GetDeltaTime
#include "../ECS/ECS.h"

namespace Ukemochi
{
    void Physics::UpdatePhysics()
    {
        for (auto& entity : m_Entities)
        {
            // Get references of entity components
            auto& trans = ECS::GetInstance().GetComponent<Transform>(entity);
            auto& rb = ECS::GetInstance().GetComponent<Rigidbody2D>(entity);
            auto& box = ECS::GetInstance().GetComponent<BoxCollider2D>(entity);

            // Get the bounding box size
            box.min = { -Ukemochi::BOUNDING_BOX_SIZE * trans.scale.x + trans.position.x,
                        -Ukemochi::BOUNDING_BOX_SIZE * trans.scale.y + trans.position.y };
            box.max = { Ukemochi::BOUNDING_BOX_SIZE * trans.scale.x + trans.position.x,
                        Ukemochi::BOUNDING_BOX_SIZE * trans.scale.y + trans.position.y };

            // Apply gravity to the entity
            //rb.velocity.y += static_cast<float>(GRAVITY * static_cast<float>(UME::g_FrameRateController.GetDeltaTime()));

            // Update position based on velocity
            if (!rb.is_kinematic)
                trans.position += rb.velocity * static_cast<float>(UME::g_FrameRateController.GetDeltaTime());
        }
    }
}