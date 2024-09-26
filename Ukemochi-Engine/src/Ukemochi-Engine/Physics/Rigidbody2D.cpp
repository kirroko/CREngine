/* Start Header
*****************************************************************/
/*!
\file       Rigidbody2D.cpp
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Sept 16, 2024
\brief      This file contains the definition of the Rigidbody2D class.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "Rigidbody2D.h"
#include "PreCompile.h"

namespace Ukemochi
{
    void UpdateRigidbody()
    {
        // if (!rb.active)
        //     return;

        // Apply gravity to the player
        // rb.velocity.y += static_cast<float>(GRAVITY * AEFrameRateControllerGetFrameTime());

        // Save previous position
        // rb.position_prev = rb.position;
        // rb.position_prev.x = rb.position.x;
        // rb.position_prev.y = rb.position.y;

        // Get the bounding box size
        // rb.bounding_box.min = {-Collision::BOUNDING_RECT_SIZE * rb.scale.x + rb.position_prev.x,
        //                        -Collision::BOUNDING_RECT_SIZE * rb.scale.y + rb.position_prev.y};
        // rb.bounding_box.max = {Collision::BOUNDING_RECT_SIZE * rb.scale.x + rb.position_prev.x,
        //                        Collision::BOUNDING_RECT_SIZE * rb.scale.y + rb.position_prev.y};

        // Update position based on velocity
        // rb.position = (rb.velocity + rb.position) * (float)AEFrameRateControllerGetFrameTime();

        // Check collision...
        // float tFirst;
        // if (Collision::CollisionIntersection_RectRect(rb.bounding_box, rb.velocity, other.bounding_box, other.velocity, tFirst))
        // {
        //     // Reset the player position
        //     rb.position = rb.initial_position;
        // }

        // // Computing the transformation matrices of the game object instances
        // for (i = 0; i < GAME_OBJ_INST_NUM_MAX; ++i)
        // {
        //     Math::Mtx33 scale{}, rot{}, trans{};
        //     pInst = sGameObjInstList + i;

        //     // skip non-active object
        //     if ((pInst->flag & FLAG_ACTIVE) == 0)
        //         continue;

        //     // Compute the scaling matrix
        //     AEMtx33Scale(&scale, pInst->scale.x, pInst->scale.y);

        //     // Compute the rotation matrix
        //     AEMtx33Rot(&rot, pInst->dirCurr);

        //     // Compute the translation matrix
        //     AEMtx33Trans(&trans, pInst->posCurr.x, pInst->posCurr.y);

        //     // Concatenate the 3 matrix in the object instance's "transform" matrix
        //     AEMtx33Concat(&pInst->transform, &rot, &scale);
        //     AEMtx33Concat(&pInst->transform, &trans, &pInst->transform);
        // }

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