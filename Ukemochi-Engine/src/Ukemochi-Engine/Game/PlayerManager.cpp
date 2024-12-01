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

            auto& trans = ECS::GetInstance().GetComponent<Transform>(entity);
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

            if (!Input::IsKeyPressed(UME_KEY_W) && !Input::IsKeyPressed(UME_KEY_S) && !Input::IsKeyPressed(UME_KEY_A) &&
                !Input::IsKeyPressed(UME_KEY_D))
            {
                anim.SetAnimation("Idle");
            }

            static bool kickAudio = false;

            if (data.attackTimer > 0.0f)
            {
                data.attackTimer -= static_cast<float>(g_FrameRateController.GetDeltaTime());
                data.canAttack = true;
                data.isAttacking = false;


                auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<
                    AudioManager>();
                if (data.currentComboHits == 3 && data.attackTimer < 1.f && kickAudio == false && audioM.GetSFXindex("Pattack3")!=-1)
                {
                    if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(
                        audioM.GetSFXindex("Pattack3")))
                    {
                        audioM.PlaySFX(audioM.GetSFXindex("Pattack3"));
                        anim.attackAnimationFinished = true;
                    }
                    kickAudio = true;
                }
            }
            else
            {
                kickAudio = false;
                data.currentComboHits = 0;
                data.canAttack = false;
                data.isAttacking = false;
                data.attackTimer = 0.0f;

                anim.isAttacking = false;
            }

            if (Input::IsKeyTriggered(UME_KEY_J))
            {
                // if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
                auto audioObj = GameObjectManager::GetInstance().GetGOByTag("AudioManager");

                // Combat logic happens?
                if (audioObj)
                {
                    AudioManager& audioM = audioObj->GetComponent<AudioManager>();

                    if (data.currentComboHits == 0 || data.canAttack)
                    {
                        data.currentComboHits++;
                        switch (data.currentComboHits % 4)
                        {
                        case 1:
                            anim.SetAnimationImmediately("Attack1");
                            if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(
                                audioM.GetSFXindex("Pattack1")) && audioM.GetSFXindex("Pattack1") != -1)
                            {
                                audioM.PlaySFX(audioM.GetSFXindex("Pattack1"));
                            }
                            break;
                        case 2:
                            anim.SetAnimationImmediately("Attack2");
                            audioM.StopSFX(audioM.GetSFXindex("Pattack1"));
                            if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(
                                audioM.GetSFXindex("Pattack2")) && audioM.GetSFXindex("Pattack2") != -1)
                            {
                                audioM.PlaySFX(audioM.GetSFXindex("Pattack2"));
                            }
                            break;
                        case 3:
                            anim.SetAnimationImmediately("Attack3");
                            break;
                        }
                        anim.isAttacking = true;

                        data.isAttacking = true;
                        data.canAttack = false; // Prevent immediate chaining of attacks
                        data.attackTimer = static_cast<float>(anim.clips[anim.currentClip].total_frames) * anim.clips[
                            anim.currentClip].frame_time;
                        // data.attackTimer = data.attackCooldown;
                    }
                }
            }

            // Update knife position
            if (sr.flipX)
            {
                auto& knife_trans = ECS::GetInstance().GetComponent<Transform>(entity + 1);
                knife_trans.position = Vec2{trans.position.x + trans.scale.x, trans.position.y};
            }
            else
            {
                auto& knife_trans = ECS::GetInstance().GetComponent<Transform>(entity + 1);
                knife_trans.position = Vec2{trans.position.x - trans.scale.x, trans.position.y};
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
            data.currentComboHits = 0;
            data.canAttack = false;
            data.attackTimer = 0.0f;
        }
    }
}
