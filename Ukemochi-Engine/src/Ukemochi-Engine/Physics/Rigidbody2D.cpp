/* Start Header
*****************************************************************/
/*!
\file       Rigidbody2D.cpp
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Sept 24, 2024
\brief      This file contains the definition of the Rigidbody2D class.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "PreCompile.h"
#include "Rigidbody2D.h"
#include "../FrameController.h"

namespace Ukemochi
{
    void Rigidbody2D::UpdateRigidbody(BoxCollider2D& box)
    {
        if (!active)
            return;

        // Apply gravity to the player
       // rb.velocity.y += static_cast<float>(GRAVITY * AEFrameRateControllerGetFrameTime());

        // Save previous position
        position_prev = position_curr;
        position_prev.x = position_curr.x;
        position_prev.y = position_curr.y;

        // Get the bounding box size
        box.min = { -Ukemochi::BOUNDING_BOX_SIZE * scale.x + position_prev.x,
                           -Ukemochi::BOUNDING_BOX_SIZE * scale.y + position_prev.y };
        box.max = { Ukemochi::BOUNDING_BOX_SIZE * scale.x + position_prev.x,
                               Ukemochi::BOUNDING_BOX_SIZE * scale.y + position_prev.y };

        // Update position based on velocity
        position_curr += velocity * static_cast<float>(UME::g_FrameRateController.GetDeltaTime());
        //position = (velocity + position) * static_cast<float>(UME::g_FrameRateController.GetDeltaTime());

        // // Computing the transformation matrices of the game object instances
        // for (unsigned int i = 0; i < GAME_OBJ_INST_NUM_MAX; ++i)
        // {
        //     Math::Mtx33 scale{}, rot{}, trans{};
        //     GameObjInst *pInst = sGameObjInstList + i;

        //     // skip non-active object
        //     if (0 == (pInst->flag & FLAG_ACTIVE))
        //         continue;

        //     Mtx33Scale(scale, pInst->scale, pInst->scale);
        //     Mtx33RotRad(rot, pInst->dirCurr);
        //     Mtx33Translate(trans, pInst->posCurr.x, pInst->posCurr.y);
        //     pInst->transform = trans * rot * scale;
        // }
    }
}