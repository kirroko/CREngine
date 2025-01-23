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

#include <dinput.h>
#include <windowsx.h>

#include "Ukemochi-Engine/FrameController.h"
#include "Ukemochi-Engine/ECS/ECS.h"
#include "Ukemochi-Engine/Factory/GameObjectManager.h"
#include "Ukemochi-Engine/Input/Input.h"

namespace Ukemochi
{
    /**
     * @brief update the PlayerManager
     */
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

            // Update knife position
            if (sr.flipX)
            {
                auto& knife_trans = ECS::GetInstance().GetComponent<Transform>(entity + 1);
                knife_trans.position = Vec3{trans.position.x + trans.scale.x, trans.position.y,0};
            }
            else
            {
                auto& knife_trans = ECS::GetInstance().GetComponent<Transform>(entity + 1);
                knife_trans.position = Vec3{trans.position.x - trans.scale.x, trans.position.y,0};
            }

            static bool kickAudio = false;
            // Handle Combo timing
            if (data.comboTimer > 0.0f)
            {
                data.comboTimer -= static_cast<float>(g_FrameRateController.GetDeltaTime());
                
                auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<
                    AudioManager>();
                // Kick
                if (data.comboState == 3 && data.comboTimer < 1.f && kickAudio == false && audioM.GetSFXindex("Pattack3")!=-1)
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
                data.comboState = -1;
            }
            
            if (data.comboIsAttacking && anim.currentClip == "Idle")
            {
                UME_ENGINE_TRACE("CAN ATTACK!");
                data.comboIsAttacking = false;
            }
            
            // Player input
            if (Input::IsKeyTriggered(UME_KEY_J))
            {
                auto audioObj = GameObjectManager::GetInstance().GetGOByTag("AudioManager");
                UME_ENGINE_ASSERT(audioObj != nullptr, "Audio Manager missing")

                AudioManager& audio = audioObj->GetComponent<AudioManager>();

                if (!data.comboIsAttacking)
                {
                    data.comboState = (data.comboState + 1) % 3;    // loop back after 3 hits
                    data.comboTimer = data.maxComboTimer;           // Reset the combo timer
                    data.comboIsAttacking = true;                        // Prevent spamming

                    UME_ENGINE_TRACE("ATTACKING! COMBO STATE: {0}",data.comboState);
                    // Perform attack
                    switch (data.comboState)
                    {
                    case 0:
                        anim.SetAnimationFromTo("Attack",0,14);
                        if (audio.GetSFXindex("Pattack1") == -1)
                            break;
                        if (ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audio.GetSFXindex("Pattack1")))
                            break;
                        audio.PlaySFX(audio.GetSFXindex("Pattack1"));
                        // Deal damage?
                        break;
                    case 1:
                        anim.SetAnimationFromTo("Attack",14,25);
                        if (audio.GetSFXindex("Pattack1") != -1) // check if it does exist
                            audio.StopSFX(audio.GetSFXindex("Pattack1"));
                        if (audio.GetSFXindex("Pattack2") == -1) // Check if it doesn't exist
                            break;
                        if (ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audio.GetSFXindex("Pattack2")))
                            break;
                        audio.PlaySFX(audio.GetSFXindex("Pattack2"));
                        break;
                    case 2:
                        anim.SetAnimationFromTo("Attack",25,46);
                        break;
                    default:
                        break;
                    }

                    UME_ENGINE_INFO("Current clip name: {0}", anim.currentClip);
                }
                // if (data.currentComboHits == 0 || data.canAttack) // first attack or can attack (Follow-up combo)
                // {
                //     data.currentComboHits++;
                //     switch (data.currentComboHits)
                //     {
                //     case 1:
                //         // anim.SetAnimationImmediately("Attack");
                //         anim.SetAnimationFromTo("Attack",0,14);
                //         if (audio.GetSFXindex("Pattack1") != -1)
                //         {
                //             if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audio.GetSFXindex("Pattack1")))
                //                 audio.PlaySFX(audio.GetSFXindex("Pattack1"));
                //         }
                //
                //         break;
                //     case 2:
                //         anim.SetAnimationFromTo("Attack",15,24);
                //         if (audio.GetSFXindex("Pattack1") != -1)
                //             audio.StopSFX(audio.GetSFXindex("Pattack1"));
                //
                //         if (audio.GetSFXindex("Pattack2") != -1)
                //         {
                //             if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audio.GetSFXindex("Pattack2")))
                //                 audio.PlaySFX(audio.GetSFXindex("Pattack2"));
                //         }
                //
                //         break;
                //     case 3:
                //         anim.SetAnimationImmediately("Attack",25);
                //         break;
                //     default:
                //         break;
                //     }
                //
                //     anim.isAttacking = true;
                //     
                //     data.canAttack = false; // Prevent immediate chaining of attacks
                //     data.attackTimer = static_cast<float>(anim.clips[anim.currentClip].total_frames) * anim.clips[anim.currentClip].frame_time;
                // }
            }
        }
    }

    /**
     * @brief Handle collision with the player
     * @param id The ID of the entity that collided with the player
     */
    void PlayerManager::OnCollisionEnter(const EntityID& id) const
    {
        UME_ENGINE_INFO("Player got hit by {0}", GameObjectManager::GetInstance().GetGO(id)->GetName());

        for (auto& entity : m_Entities)
        {
            // which is one entity...
            auto& data = ECS::GetInstance().GetComponent<Player>(entity);
            auto& anim = ECS::GetInstance().GetComponent<Animation>(entity);

            anim.SetAnimationUninterrupted("Hurt");
            data.comboState = 0;
            data.canAttack = false;
            data.comboTimer = 0.0f;
        }
    }
}
