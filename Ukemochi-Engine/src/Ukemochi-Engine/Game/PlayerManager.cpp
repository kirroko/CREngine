/* Start Header ************************************************************************/
/*!
\file       PlayerManager.cpp
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date       Nov 21, 2024
\brief      This file is a temporary solution to manage player's logic and should replace to the C# script as soon as possible. 

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#include "PreCompile.h"
#include "PlayerManager.h"

#include "Ukemochi-Engine/FrameController.h"
#include "Ukemochi-Engine/ECS/ECS.h"
#include "Ukemochi-Engine/Factory/GameObjectManager.h"
#include "Ukemochi-Engine/Input/Input.h"

namespace Ukemochi
{
    void PlayerManager::Update() const
    {
        for (auto& entity : m_Entities)
        {
            auto& data = ECS::GetInstance().GetComponent<Player>(entity);
            // I know that this entity will have transform, rigidbody2d and spriteRender, but it's not implicitly stated when setting the signature
            if (!ECS::GetInstance().HasComponent<Rigidbody2D>(entity))
            {
                UME_ENGINE_WARN("Entity {0} doesn't have Rigidbody2D component", entity);
                return;
            }
            if (!ECS::GetInstance().HasComponent<Animation>(entity))
            {
                UME_ENGINE_WARN("Entity {0} doesn't have Animation component", entity);
                return;
            }
            if (!ECS::GetInstance().HasComponent<SpriteRender>(entity))
            {
                UME_ENGINE_WARN("Entity {0} doesn't have SpriteRender component", entity);
                return;
            }
            auto& rb = ECS::GetInstance().GetComponent<Rigidbody2D>(entity);
            auto& anim = ECS::GetInstance().GetComponent<Animation>(entity);
            auto& sr = ECS::GetInstance().GetComponent<SpriteRender>(entity);

            if (Input::IsKeyPressed(UME_KEY_W))
            {
                rb.force.y = data.playerForce;
                anim.SetAnimation("Running");
            }
            else if (Input::IsKeyPressed(UME_KEY_S))
            {
                rb.force.y = -data.playerForce;
                anim.SetAnimation("Running");
            }
            else
            {
                rb.force.y = 0.f;
            }

            if (Input::IsKeyPressed(UME_KEY_A))
            {
                rb.force.x = -data.playerForce;
                anim.SetAnimation("Running");
                sr.flipX = false;
            }
            else if (Input::IsKeyPressed(UME_KEY_D))
            {
                rb.force.x = data.playerForce;
                anim.SetAnimation("Running");
                sr.flipX = true;
            }
            else
            {
                rb.force.x = 0.f;
            }

            if (!Input::IsKeyPressed(UME_KEY_W) && !Input::IsKeyPressed(UME_KEY_S) && !Input::IsKeyPressed(UME_KEY_A) && !Input::IsKeyPressed(UME_KEY_D))
            {
                anim.SetAnimation("Idle");
            }

            if (data.attackTimer > 0.0f)
            {
                data.attackTimer -= static_cast<float>(g_FrameRateController.GetDeltaTime());
                data.canAttack = true;
            }
            else
            {
                data.currentComboHits = 0;
                data.canAttack = false;
                data.attackTimer = 0.0f;
            }

            if (Input::IsMouseButtonTriggered(UME_MOUSE_BUTTON_1))
            {
                // Combat logic happens?
                if (data.currentComboHits == 0 || data.canAttack)
                {
                    data.currentComboHits++;
                    switch (data.currentComboHits)
                    {
                    case 1:
                        anim.SetAnimationImmediately("Attack1");
                        break;
                    case 2:
                        anim.SetAnimationImmediately("Attack2");
                        
                        break;
                    case 3:
                        anim.SetAnimationImmediately("Attack3");
                        break;
                    default:
                        anim.SetAnimationImmediately("Idle");
                        break;
                    }
                    anim.isAttacking = true;
                    data.canAttack = false; // Prevent immediate chaining of attacks
                    data.attackTimer = static_cast<float>(anim.clips[anim.currentClip].total_frames) * anim.clips[anim.currentClip].frame_time;
                    // data.attackTimer = data.attackCooldown;
                }
            }
        }
    }

    void PlayerManager::OnCollisionEnter(const EntityID& id) const
    {
        UME_ENGINE_INFO("Player got hit by {0}", GameObjectManager::GetInstance().GetGO(id)->GetName());

        for (auto& entity : m_Entities)
        {
            // which is one entity...
            auto& data = ECS::GetInstance().GetComponent<Player>(entity);
            auto& anim = ECS::GetInstance().GetComponent<Animation>(entity);

            anim.SetAnimationUninterrupted("Hurt");
        }
    }
}
