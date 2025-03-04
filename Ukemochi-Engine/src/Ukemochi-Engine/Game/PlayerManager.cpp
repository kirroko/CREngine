/* Start Header ************************************************************************/
/*!
\file       PlayerManager.cpp
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu (90%)
\co-authors HURNG Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu (10%)
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
    void PlayerManager::PlayersMovement(Rigidbody2D& rb, SpriteRender& sr, const Player& data) const
    {
        if (Input::IsKeyPressed(UME_KEY_W))
            rb.force.y = data.playerForce;
        else if (Input::IsKeyPressed(UME_KEY_S))
            rb.force.y = -data.playerForce;
        else
            rb.force.y = 0.f;

        if (Input::IsKeyPressed(UME_KEY_A))
        {
            rb.force.x = -data.playerForce;
            sr.flipX = false;
        }
        else if (Input::IsKeyPressed(UME_KEY_D))
        {
            rb.force.x = data.playerForce;
            sr.flipX = true;
        }
        else
        {
            rb.force.x = 0.f;
        }
    }

    std::string PlayerManager::SoulAnimation(const PlayerSoul& soulData, std::string clip) const
    {
        std::string temp = "";
        switch (soulData.current_soul)  // NOLINT(clang-diagnostic-switch-enum)
        {
        case EMPTY: // Grey
            temp += std::move(clip);
            break;
        case FISH: // Blue
            temp.push_back('b');
            temp += std::move(clip);
            break;
        case WORM: // Red
            temp.push_back('r');
            temp += std::move(clip);
            break;
        default:
            break;
        }

        return temp;
    }

    bool PlayerManager::CheckIfIdle(const Animation& anim) const
    {
        std::string check = anim.currentClip;
        return check.find("Idle") != std::string::npos;
    }

    /**
     * @brief update the PlayerManager
     */
    void PlayerManager::Update() const
    {
        // Update the player manager based on the number of steps
        for (int step = 0; step < g_FrameRateController.GetCurrentNumberOfSteps(); ++step)
        {
            for (auto& entity : m_Entities)
            {
                auto& data = ECS::GetInstance().GetComponent<Player>(entity);
                auto& soulData = ECS::GetInstance().GetComponent<PlayerSoul>(entity);

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

                auto& shadow_trans = GameObjectManager::GetInstance().GetGOByName("Player_Shadow")->GetComponent<Transform>();

                static bool playerDeadSoundPlayed = false;

                // Check if Mochi is dead
                if (data.currentHealth <= 0)
                {
                    auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                    if (!playerDeadSoundPlayed && audioM.GetSFXindex("PlayerDead") != -1)
                    {
                        audioM.PlaySFX(audioM.GetSFXindex("PlayerDead"));
                        playerDeadSoundPlayed = true; // Prevent it from playing again
                    }
                    // Trigger player death animation
                    anim.SetAnimation(SoulAnimation(soulData, "Death"));
                    data.isDead = true;

                    // Stop Mochi's movement
                    rb.force = Vec2{ 0,0 };
                    rb.velocity = Vec2{ 0,0 };

                    return;
                }

                playerDeadSoundPlayed = false; // Reset the flag if the player is not dead

                // Allow Mochi to move if not attacking or casting an ability
                if (!data.comboIsAttacking && !soulData.is_casting)
                    PlayersMovement(rb, sr, data);

                // Move the offset player's shadow
                if (!sr.flipX)
                    shadow_trans.position = Vector3D(trans.position.x - shadow_trans.scale.x * 0.25f, trans.position.y - shadow_trans.scale.y * 0.55f, trans.position.z);
                else
                    shadow_trans.position = Vector3D(trans.position.x + shadow_trans.scale.x * 0.25f, trans.position.y - shadow_trans.scale.y * 0.55f, trans.position.z);

                if ((Input::IsKeyPressed(UME_KEY_W) || Input::IsKeyPressed(UME_KEY_S) || Input::IsKeyPressed(UME_KEY_A) || Input::IsKeyPressed(UME_KEY_D))
                    && !data.comboIsAttacking)
                    anim.SetAnimation(SoulAnimation(soulData, "Running"));
                // anim.SetAnimation("Running");
                else
                    anim.SetAnimation(SoulAnimation(soulData, "Idle"));
                // anim.SetAnimation("Idle");

                // Play the running sound only at frame 2
                static bool runningSoundPlayed = false;
                if (anim.currentClip.find("Running") != std::string::npos)  // Only when running animation is active
                {
                    int currentFrame = anim.GetCurrentFrame(); // Assuming you have a GetCurrentFrame method

                    static int lastRunningSoundIndex = 0;

                    // 6 or 7 for current frame
                    if ((currentFrame == 3 || currentFrame == 8) && !runningSoundPlayed)
                    {
                        // Assuming you have an audio manager and running sound index
                        auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                        std::vector<int> runningSounds = {
                            audioM.GetSFXindex("Running1"),
                            audioM.GetSFXindex("Running2"),
                            audioM.GetSFXindex("Running3"),
                            audioM.GetSFXindex("Running4"),
                            audioM.GetSFXindex("Running5"),
                            audioM.GetSFXindex("Running6"),
                            audioM.GetSFXindex("Running7"),
                            audioM.GetSFXindex("Running8"),
                            audioM.GetSFXindex("Running9"),
                            audioM.GetSFXindex("Running10"),
                            audioM.GetSFXindex("Running11")
                        };
                        // Add this to track when sounds were last played (initialize this elsewhere)
                        static std::vector<int> lastPlayedCounts(runningSounds.size(), 0);

                        // Filter out any invalid indices (-1) and recently played sounds
                        std::vector<int> validSounds;
                        for (int i = 0; i < runningSounds.size(); i++) {
                            // Add only valid sounds that haven't been played in the last 5 rounds
                            if (runningSounds[i] != -1 && lastPlayedCounts[i] >= 5) {
                                validSounds.push_back(runningSounds[i]);
                            }
                        }

                        // If no valid sounds are available, use any that aren't -1
                        if (validSounds.empty()) {
                            for (int i = 0; i < runningSounds.size(); i++) {
                                if (runningSounds[i] != -1) {
                                    validSounds.push_back(runningSounds[i]);
                                }
                            }
                        }

                        if (!validSounds.empty()) {
                            // Random selection
                            int randomIndex = rand() % validSounds.size();
                            int selectedSound = validSounds[randomIndex];

                            // Find the original index to update the counter
                            for (int i = 0; i < runningSounds.size(); i++) {
                                if (runningSounds[i] == selectedSound) {
                                    lastPlayedCounts[i] = 0; // Reset counter for the played sound
                                }
                            }

                            // Play the selected sound
                            audioM.PlaySFX(selectedSound);

                            // Increment counters for all sounds
                            for (int i = 0; i < lastPlayedCounts.size(); i++) {
                                lastPlayedCounts[i]++;
                            }
                        }

                        runningSoundPlayed = true; // Prevent it from playing again at the same frame
                    }
                    else if ((currentFrame != 3 && currentFrame != 8))
                    {
                        runningSoundPlayed = false; // Reset when leaving frame 2
                    }
                }

                // Update knife position
                auto& knife_trans = GameObjectManager::GetInstance().GetGOByName("Knife")->GetComponent<Transform>();
                if (sr.flipX)
                    knife_trans.position = Vec3{ trans.position.x + trans.scale.x, trans.position.y,0 };
                else
                    knife_trans.position = Vec3{ trans.position.x - trans.scale.x, trans.position.y,0 };

                static bool kickAudio = false;
                // Handle Combo timing
                if (data.comboTimer > 0.0f)
                {
                    data.comboTimer -= static_cast<float>(g_FrameRateController.GetFixedDeltaTime());

                    auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<
                        AudioManager>();
                    // Kick
                    if (data.comboState == 2 && anim.current_frame >= 29 && kickAudio == false && audioM.GetSFXindex("Pattack3") != -1)
                    {
                        if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(
                            audioM.GetSFXindex("Pattack3")))
                        {
                            //audioM.PlaySFX(audioM.GetSFXindex("Pattack3"));
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

                if (data.comboIsAttacking && CheckIfIdle(anim))
                    data.comboIsAttacking = false;

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

                        rb.force = Vec2(0, 0);

                        UME_ENGINE_TRACE("ATTACKING! COMBO STATE: {0}", data.comboState);
                        // Perform attack
                        switch (data.comboState)
                        {
                        case 0:
                            anim.isReverse = true;
                            anim.SetAnimationFromTo(SoulAnimation(soulData, "Attack"), 11, 0);

                            // anim.SetAnimationFromTo("Attack", 0, 14);
                            if (audio.GetSFXindex("Pattack1") == -1)
                                break;
                            if (ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audio.GetSFXindex("Pattack1")))
                                break;
                            audio.PlaySFX(audio.GetSFXindex("Pattack1"));
                            // Deal damage?
                            break;
                        case 1:
                            anim.SetAnimationFromTo(SoulAnimation(soulData, "Attack"), 14, 25);
                            // anim.SetAnimationFromTo("Attack", 14, 25);
                            if (audio.GetSFXindex("Pattack1") != -1) // check if it does exist
                                audio.StopSFX(audio.GetSFXindex("Pattack1"));
                            if (audio.GetSFXindex("Pattack2") == -1) // Check if it doesn't exist
                                break;
                            if (ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audio.GetSFXindex("Pattack2")))
                                break;
                            audio.PlaySFX(audio.GetSFXindex("Pattack2"));
                            anim.attackAnimationFinished = false;
                            kickAudio = false;
                            break;
                        case 2:
                            anim.SetAnimationFromTo(SoulAnimation(soulData, "Attack"), 30, 46);
                            // anim.SetAnimationFromTo("Attack", 25, 46);
                            break;
                        default:
                            break;
                        }

                        UME_ENGINE_INFO("Current clip name: {0}", anim.currentClip);
                    }
                }
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
            auto& soulData = ECS::GetInstance().GetComponent<PlayerSoul>(entity);
            
            // Check if the player is already dead
            if (data.currentHealth <= 0 || data.isDead)
            {
                return; // Exit early if the player is dead
            }
            
            anim.SetAnimationUninterrupted(SoulAnimation(soulData,"Hurt"));

            if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
            {
                auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();

                if (audioM.GetSFXindex("PlayerHurt") != -1)
                {
                    if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("PlayerHurt")))
                    {
                        audioM.PlaySFX(audioM.GetSFXindex("PlayerHurt"));
                    }
                }
            }

            // Sync hurt audio with animation
            static bool hurtSoundPlayed = false;
            if (anim.currentClip == "Hurt")  // Only when hurt animation is active
            {
                int currentFrame = anim.GetCurrentFrame(); // Assuming you have a GetCurrentFrame method

                // Play hurt sound at specific frames (e.g., frame 5)
                if (currentFrame == 5 && !hurtSoundPlayed)
                {
                    auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                    int hurtSoundIndex = audioM.GetSFXindex("PlayerHurt"); // Replace with the actual sound name
                    if (hurtSoundIndex != -1)
                    {
                        audioM.PlaySFX(hurtSoundIndex);
                    }
                    hurtSoundPlayed = true; // Prevent it from playing again at the same frame
                }
                else if (currentFrame != 5)
                {
                    hurtSoundPlayed = false; // Reset when leaving frame 5
                }
            }
            // anim.SetAnimationUninterrupted("Hurt");
            data.comboState = 0;
            data.canAttack = false;
            data.comboTimer = 0.0f;
        }
    }
}
