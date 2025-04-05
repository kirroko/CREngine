/* Start Header ************************************************************************/
/*!
\file       SoulManager.cpp
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu (87%)
\co-authors HURNG Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu (10%)
            Tan Si Han, t.sihan, 2301264, t.sihan\@digipen.edu (3%)
\date       Mar 27, 2025
\brief      This file contains the definition of the SoulManager which handles the soul system.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "SoulManager.h"                  // for forward declaration
#include "../Input/Input.h"               // for button inputs
#include "../Factory/GameObjectManager.h" // for game object tag
#include "../FrameController.h"           // for GetCurrentNumberOfSteps, GetFixedDeltaTime
#include "../Graphics/UIButtonManager.h"  // for button effect
#include "../Game/BossManager.h"          // for spawning monster
#include "../SceneManager.h"              // for cheat mode
#include "GLFW/glfw3.h"                   // for controller support

namespace Ukemochi
{
    /*!***********************************************************************
    \brief
     Initialize the soul manager, finding the player and ability-related entities.
    *************************************************************************/
    void SoulManager::Init()
    {
        // Find the player, souls, fish ability and worm abilty entities
        player = soul = UI_red_soul = UI_blue_soul = soul_orb_clone = fish_ability = worm_ability = static_cast<EntityID>(-1);
        for (auto const& entity : m_Entities)
        {
            if (GameObjectManager::GetInstance().GetGO(entity)->GetTag() == "Player")
            {
                player = entity; // Player ID is found
            }
            else if (GameObjectManager::GetInstance().GetGO(entity)->GetTag() == "Soul")
            {
                soul = entity; // Floating soul ID is found
                GameObjectManager::GetInstance().GetGO(entity)->SetActive(false);
            }
            else if (GameObjectManager::GetInstance().GetGO(entity)->GetTag() == "UI_Red_Soul")
            {
                UI_red_soul = entity; // Red soul ID is found
                GameObjectManager::GetInstance().GetGO(entity)->SetActive(false);
            }
            else if (GameObjectManager::GetInstance().GetGO(entity)->GetTag() == "UI_Blue_Soul")
            {
                UI_blue_soul = entity; // Blue soul ID is found
                GameObjectManager::GetInstance().GetGO(entity)->SetActive(false);
            }
            else if (GameObjectManager::GetInstance().GetGO(entity)->GetTag() == "OrbClone")
            {
                soul_orb_clone = entity; // Soul orb clone ID is found
            }
            else if (GameObjectManager::GetInstance().GetGO(entity)->GetTag() == "FishAbility")
            {
                // Fish ability ID is found, disable fish ability object
                fish_ability = entity;
                GameObjectManager::GetInstance().GetGO(entity)->SetActive(false);
            }
            else if (GameObjectManager::GetInstance().GetGO(entity)->GetTag() == "WormAbility")
            {
                // Worm ability ID is found, disable worm ability object
                worm_ability = entity;
                GameObjectManager::GetInstance().GetGO(entity)->SetActive(false);
            }
        }
    }

    /*!***********************************************************************
    \brief
     Update the soul manager, handling player inputs and soul decay.
    *************************************************************************/
    void SoulManager::Update()
    {
        // Check if player, souls, fish and worm IDs is valid
        if (player == -1 || soul == -1 || UI_red_soul == -1 || UI_blue_soul == -1
            || soul_orb_clone == -1 || fish_ability == -1 || worm_ability == -1)
            return;

        // Check if player is alive
        auto& player_data = ECS::GetInstance().GetComponent<Player>(player);
        if (player_data.currentHealth <= 0)
        {
            // If player is dead, disable floating soul
            GameObjectManager::GetInstance().GetGO(soul)->SetActive(false);
            return;
        }

        // Soul Switch Key Press
        if (Input::IsKeyTriggered(UME_KEY_Q) || Input::IsGamepadButtonTriggered(GLFW_JOYSTICK_1, GLFW_GAMEPAD_BUTTON_LEFT_BUMPER))
        {
            SwitchSouls();

            // Trigger button darken effect
            auto& qButton = ECS::GetInstance().GetSystem<UIButtonManager>()->buttons["soul change"];
            if (qButton)
                qButton->triggerDarkenEffect();
        }

        // Soul Ability Key Press
        if (Input::IsKeyTriggered(UME_KEY_K) || Input::IsGamepadButtonTriggered(GLFW_JOYSTICK_1, GLFW_GAMEPAD_BUTTON_TRIANGLE))
        {
            UseSoulAbility();

            // Trigger button darken effect
            auto& fButton = ECS::GetInstance().GetSystem<UIButtonManager>()->buttons["game ability"];
            if (fButton)
                fButton->triggerDarkenEffect();
        }

        // Update the soul system based on the number of steps
        for (int step = 0; step < g_FrameRateController.GetCurrentNumberOfSteps(); ++step)
        {
            // Handle enemy projectile (placeholder)
            HandleEnemyProjectile();

            if (!SceneManager::GetInstance().enable_cheatmode) // Dont decay soul if cheat mode is enabled
                HandleSoulDecay(); // Handle soul bar decay over time

            // Handle skill effects over time
            HandleSkillEffects();

            // Handle floating soul
            HandleFloatingSoul();

            // Handle soul orbs
            HandleOrbs();
        }
    }

    /*!***********************************************************************
    \brief
     Harvest soul into the player's soul bar based on the given soul type and amount.
    \param[in] soul_type
     The type of soul to harvest (e.g., FISH, WORM).
    \param[in] amount
     The amount of soul to harvest.
    *************************************************************************/
    void SoulManager::HarvestSoul(const SoulType soul_type, const float amount)
    {
        if (soul_type < 0 || soul_type >= NUM_OF_SOULS)
            return;

        auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);

        // Harvest soul into the soul bar
        if (player_soul.soul_bars[soul_type] + amount > MAX_SOUL_BAR)
            player_soul.soul_bars[soul_type] = MAX_SOUL_BAR;
        else
            player_soul.soul_bars[soul_type] += amount;

        // Gain a soul charge if the soul bar is filled
        if (player_soul.soul_bars[soul_type] >= MAX_SOUL_BAR && player_soul.soul_charges[soul_type] < MAX_SOUL_CHARGES)
        {
            ++player_soul.soul_charges[soul_type];

            // Enable floating/red/blue soul
            if (player_soul.current_soul == soul_type)
                GameObjectManager::GetInstance().GetGO(soul)->SetActive(true);
            if (soul_type == FISH)
                GameObjectManager::GetInstance().GetGO(UI_blue_soul)->SetActive(true);
            else if (soul_type == WORM)
                GameObjectManager::GetInstance().GetGO(UI_red_soul)->SetActive(true);
        }
    }

    /*!***********************************************************************
    \brief
     Spawn a soul orb based on the enemy's position.
    \param[in] soul_type
     The type of soul to harvest (e.g., FISH, WORM).
    \param[in] amount
     The amount of soul to harvest.
    \param[in] spawn_position
     The position of the dead enemy.
    *************************************************************************/
    void SoulManager::SpawnSoulOrb(const SoulType soul_type, const float amount, const Vec3 spawn_position)
    {
        // Check if the souls IDs is valid
        if (UI_red_soul == -1 || UI_blue_soul == -1 || soul_orb_clone == -1)
            return;

        GameObject* orb_clone = GameObjectManager::GetInstance().GetGO(soul_orb_clone);
        if (orb_clone != nullptr)
        {
            GameObject& new_orb = GameObjectManager::GetInstance().CloneObject(*orb_clone, "Orb", "Orb");

            // Set the orb's soul type, amount and target position
            new_orb.GetComponent<SoulOrb>().orb_type = soul_type;
            new_orb.GetComponent<SoulOrb>().soul_amount = amount;
            if (soul_type == FISH)
                new_orb.GetComponent<SoulOrb>().target_position = ECS::GetInstance().GetComponent<Transform>(UI_blue_soul).position;
            else if (soul_type == WORM)
                new_orb.GetComponent<SoulOrb>().target_position = ECS::GetInstance().GetComponent<Transform>(UI_red_soul).position;

            // Set the orb's initial position to the enemy's death position
            new_orb.GetComponent<Transform>().position = spawn_position;

            // Activate the orb and add it to the entity list
            new_orb.SetActive(true);
        }
    }

    /*!***********************************************************************
    \brief
     Switch between the player's souls (FISH, WORM, or EMPTY) based on available soul bars.
    *************************************************************************/
    void SoulManager::SwitchSouls()
    {
        auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);
        auto& player_animator = ECS::GetInstance().GetComponent<Animation>(player);
        auto& soul_animator = ECS::GetInstance().GetComponent<Animation>(soul);

        // Skip if Mochi is still casting an animation
        if (player_soul.is_casting || player_animator.isAttacking)
            return;

        // Play different hit sounds based on enemy type
        if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
        {
            auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
            if (audioM.GetSFXindex("SwapSoul") != -1)
                audioM.PlaySFX(audioM.GetSFXindex("SwapSoul"));
        }

        // Currently in EMPTY soul, switch to FISH or WORM souls if available
        if (player_soul.current_soul == SoulType::EMPTY)
        {
            if (player_soul.soul_bars[FISH] > 0)
            {
                player_soul.current_soul = FISH;
                player_animator.SetAnimationUninterrupted("SwitchNB");
                soul_animator.SetAnimation("BlueFlame");
                if (player_soul.soul_charges[FISH] > 0)
                    GameObjectManager::GetInstance().GetGO(soul)->SetActive(true);
                else
                    GameObjectManager::GetInstance().GetGO(soul)->SetActive(false);
            }
            else if (player_soul.soul_bars[WORM] > 0)
            {
                player_soul.current_soul = WORM;
                player_animator.SetAnimationUninterrupted("SwitchNR");
                soul_animator.SetAnimation("RedFlame");
                if (player_soul.soul_charges[WORM] > 0)
                    GameObjectManager::GetInstance().GetGO(soul)->SetActive(true);
                else
                    GameObjectManager::GetInstance().GetGO(soul)->SetActive(false);
            }
        }
        // Currently in FISH soul, switch to WORM soul if available else EMPTY soul
        else if (player_soul.current_soul == FISH)
        {
            if (player_soul.soul_bars[WORM] > 0)
            {
                player_soul.current_soul = WORM;
                player_animator.SetAnimationUninterrupted("SwitchBR");
                soul_animator.SetAnimation("RedFlame");
                if (player_soul.soul_charges[WORM] > 0)
                    GameObjectManager::GetInstance().GetGO(soul)->SetActive(true);
                else
                    GameObjectManager::GetInstance().GetGO(soul)->SetActive(false);
            }
            else
            {
                player_soul.current_soul = EMPTY;
                player_animator.SetAnimationUninterrupted("SwitchBN");
                GameObjectManager::GetInstance().GetGO(soul)->SetActive(false);
            }
        }
        // Currently in WORM soul, switch to FISH soul if available else EMPTY soul
        else if (player_soul.current_soul == WORM)
        {
            if (player_soul.soul_bars[FISH] > 0)
            {
                player_soul.current_soul = FISH;
                player_animator.SetAnimationUninterrupted("SwitchRB");
                soul_animator.SetAnimation("BlueFlame");
                if (player_soul.soul_charges[FISH] > 0)
                    GameObjectManager::GetInstance().GetGO(soul)->SetActive(true);
                else
                    GameObjectManager::GetInstance().GetGO(soul)->SetActive(false);
            }
            else
            {
                player_soul.current_soul = EMPTY;
                player_animator.SetAnimationUninterrupted("SwitchRN");
                GameObjectManager::GetInstance().GetGO(soul)->SetActive(false);
            }
        }

        // Don't let Mochi move while switching souls
        player_soul.is_casting = true;
        ECS::GetInstance().GetComponent<Rigidbody2D>(player).force = Vec2{ 0,0 };
    }

    /*!***********************************************************************
    \brief
     Use the current soul ability, if the player has enough charges.
    *************************************************************************/
    void SoulManager::UseSoulAbility()
    {
        auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);

        // Check if the skill is ready or casting an ability
        if (!player_soul.skill_ready || player_soul.is_casting)
            return;

        // Use the FISH ability if there are enough FISH charges
        if (player_soul.current_soul == FISH && player_soul.soul_charges[FISH] > 0)
            UseFishAbility();
        // Use the WORM ability if there are enough WORM charges
        else if (player_soul.current_soul == WORM && player_soul.soul_charges[WORM] > 0)
            UseWormAbility();
    }

    /*!***********************************************************************
    \brief
     Contain the logic for the fish ability.
     Trigger fish animations, find nearest enemy position and use soul charge.
    *************************************************************************/
    void SoulManager::UseFishAbility()
    {
        // Play different hit sounds based on enemy type
        if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
        {
            auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
            if (audioM.GetSFXindex("FishSpecial") != -1)
                audioM.PlaySFX(audioM.GetSFXindex("FishSpecial"));
        }

        // Trigger player fish casting animation
        auto& player_animator = ECS::GetInstance().GetComponent<Animation>(player);
        player_animator.SetAnimationUninterrupted("FishSkill");

        // Trigger fish AOE effect at the nearest enemy
        auto& fish_transform = ECS::GetInstance().GetComponent<Transform>(fish_ability);
        fish_transform.position = FindNearestEnemyPosition();

        // Trigger fish animation and enable GO
        auto& fish_animator = ECS::GetInstance().GetComponent<Animation>(fish_ability);
        fish_animator.SetAnimation("FishAbilitySpawn");
        GameObjectManager::GetInstance().GetGO(fish_ability)->SetActive(true);

        // Decrease soul charge and reset skill stats
        auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);
        if (!SceneManager::GetInstance().enable_cheatmode) // Dont use soul charge if cheat mode is enabled
        {
            --player_soul.soul_charges[player_soul.current_soul];
            GameObjectManager::GetInstance().GetGO(soul)->SetActive(false); // Disable floating soul
            GameObjectManager::GetInstance().GetGO(UI_blue_soul)->SetActive(false); // Disable blue soul
        }
        player_soul.skill_ready = false;
        player_soul.skill_timer = 0.f;

        // Don't let Mochi move while casting soul ability
        player_soul.is_casting = true;
        ECS::GetInstance().GetComponent<Rigidbody2D>(player).force = Vec2{ 0,0 };
    }

    /*!***********************************************************************
    \brief
     Contain the logic for the worm ability.
     Trigger worm animations, find nearest enemy position and use soul charge.
    *************************************************************************/
    void SoulManager::UseWormAbility()
    {
        // Play different hit sounds based on enemy type
        if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
        {
            auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
            if (audioM.GetSFXindex("WormSpecial") != -1)
                audioM.PlaySFX(audioM.GetSFXindex("WormSpecial"));
        }

        // Trigger player worm casting animation
        auto& player_animator = ECS::GetInstance().GetComponent<Animation>(player);
        player_animator.SetAnimationUninterrupted("WormSkill");

        // Trigger worm web effect at the nearest enemy
        auto& worm_transform = ECS::GetInstance().GetComponent<Transform>(worm_ability);
        worm_transform.position = FindNearestEnemyPosition();

        // Trigger worm animation and enable GO
        auto& worm_animator = ECS::GetInstance().GetComponent<Animation>(worm_ability);
        worm_animator.SetAnimation("WormAbilitySpawn");
        GameObjectManager::GetInstance().GetGO(worm_ability)->SetActive(true);

        // Decrease soul charge and reset skill stats
        auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);
        if (!SceneManager::GetInstance().enable_cheatmode) // Dont use soul charge if cheat mode is enabled
        {
            --player_soul.soul_charges[player_soul.current_soul];
            GameObjectManager::GetInstance().GetGO(soul)->SetActive(false); // Disable floating soul
            GameObjectManager::GetInstance().GetGO(UI_red_soul)->SetActive(false); // Disable red soul
        }
        player_soul.skill_ready = false;
        player_soul.skill_timer = 0.f;

        // Don't let Mochi move while casting soul ability
        player_soul.is_casting = true;
        ECS::GetInstance().GetComponent<Rigidbody2D>(player).force = Vec2{ 0,0 };
    }

    /*!***********************************************************************
    \brief
     Contain the logic for applying the fish ability.
     Mochi's Fish Ability and Enemy (Enemy takes huge damage).
    *************************************************************************/
    void SoulManager::ApplyFishAbility(const EntityID& enemy)
    {
        if (GameObjectManager::GetInstance().GetGO(enemy)->GetTag() == "Enemy")
        {
            // Get references of the player soul and enemy
            auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);
            auto& enemy_data = ECS::GetInstance().GetComponent<Enemy>(enemy);

            // Deal damage to the enemy
            if (!enemy_data.wasHit)
            {
                enemy_data.TakeDamage(player_soul.skill_damages[player_soul.current_soul]);
                ECS::GetInstance().GetComponent<SpriteRender>(enemy).color = Vec3(1.f, 0.f, 0.f);

                // Trigger enemy hurt animation
                if (enemy_data.health > 0)
                    ECS::GetInstance().GetComponent<Animation>(enemy).SetAnimationUninterrupted("Hurt");
            }
        }
        else if (GameObjectManager::GetInstance().GetGO(enemy)->GetTag() == "Dummy")
        {
            // Trigger dummy hit animation
            auto& dummy_anim = ECS::GetInstance().GetComponent<Animation>(enemy);
            dummy_anim.RestartAnimation();
        }
    }

    /*!***********************************************************************
    \brief
     Contain the logic for applying the worm ability.
     Mochi's Worm Ability and Enemy (Enemy gets trap in the web).
    *************************************************************************/
    void SoulManager::ApplyWormAbility(const EntityID& enemy)
    {
        if (GameObjectManager::GetInstance().GetGO(enemy)->GetTag() == "Enemy")
        {
            // Get reference of the enemy
            auto& enemy_rb = ECS::GetInstance().GetComponent<Rigidbody2D>(enemy);
            auto& enemy_data = ECS::GetInstance().GetComponent<Enemy>(enemy);

            // Trigger enemy hurt animation
            if (enemy_data.health > 0)
                ECS::GetInstance().GetComponent<Animation>(enemy).SetAnimationUninterrupted("Hurt");

            // Stop the enemy's movement
            enemy_rb.force = Vec2{ 0,0 };
            enemy_rb.velocity = Vec2{ 0,0 };
        }
        else if (GameObjectManager::GetInstance().GetGO(enemy)->GetTag() == "Dummy")
        {
            // Trigger dummy hit animation
            auto& dummy_anim = ECS::GetInstance().GetComponent<Animation>(enemy);
            dummy_anim.RestartAnimation();
        }
    }

    /*!***********************************************************************
    \brief
     Find the position of the nearest enemy relative to the player.
    \return
     The position of the nearest enemy or the player's position if no enemy exist.
    *************************************************************************/
    Vector3D SoulManager::FindNearestEnemyPosition()
    {
        Vec3 player_position = ECS::GetInstance().GetComponent<Transform>(player).position;
        Vec3 nearest_enemy_position = player_position;
        float min_distance = std::numeric_limits<float>::max();

        // Search through the entity list for the nearest enemy
        for (auto const& entity : m_Entities)
        {
            if (GameObjectManager::GetInstance().GetGO(entity)->GetTag() == "Enemy"
                && GameObjectManager::GetInstance().GetGO(entity)->GetActive())
            {
                if (GameObjectManager::GetInstance().GetGO(entity)->HasComponent<Enemy>())
                {
                    if (GameObjectManager::GetInstance().GetGO(entity)->GetComponent<Enemy>().health > 0.f)
                    {
                        Vec3 enemy_position = ECS::GetInstance().GetComponent<Transform>(entity).position;
                        Vec2 enemy_scale = ECS::GetInstance().GetComponent<Transform>(entity).scale;
                        float distance = Vec3Length(enemy_position - player_position);

                        if (distance < min_distance)
                        {
                            min_distance = distance;
                            nearest_enemy_position.x = enemy_position.x;
                            nearest_enemy_position.y = enemy_position.y + enemy_scale.y * 0.1f;
                        }
                    }
                }
            }
            else if (GameObjectManager::GetInstance().GetGO(entity)->GetTag() == "Dummy"
                && GameObjectManager::GetInstance().GetGO(entity)->GetActive())
            {
                Vec3 enemy_position = ECS::GetInstance().GetComponent<Transform>(entity).position;
                Vec2 enemy_scale = ECS::GetInstance().GetComponent<Transform>(entity).scale;
                float distance = Vec3Length(enemy_position - player_position);

                if (distance < min_distance)
                {
                    min_distance = distance;
                    nearest_enemy_position.x = enemy_position.x;
                    nearest_enemy_position.y = enemy_position.y + enemy_scale.y * 0.1f;
                }
            }
        }

        // Return the nearest enemy position
        return nearest_enemy_position;
    }

    /*!***********************************************************************
    \brief
     Find a list of nearby enemies within the skill range of the player's current soul ability.
    \return
     A vector of EntityIDs representing the enemies within the ability's range.
    *************************************************************************/
    std::vector<EntityID> SoulManager::FindNearbyEnemies()
    {
        // Get reference of the player soul
        auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);

        // Get the current skill position and range
        Vec3 skill_position = ECS::GetInstance().GetComponent<Transform>(player).position;
        float skill_range = 0.0f;

        if (player_soul.current_soul == FISH)
        {
            skill_position = ECS::GetInstance().GetComponent<Transform>(fish_ability).position;
            skill_range = ECS::GetInstance().GetComponent<Transform>(fish_ability).scale.x;
        }
        else if (player_soul.current_soul == WORM)
        {
            skill_position = ECS::GetInstance().GetComponent<Transform>(worm_ability).position;
            skill_range = ECS::GetInstance().GetComponent<Transform>(worm_ability).scale.x;
        }

        // Search through the entity list for nearby enemies within the range
        std::vector<EntityID> nearby_enemies;
        for (auto const& entity : m_Entities)
        {
            if ((GameObjectManager::GetInstance().GetGO(entity)->GetTag() == "Enemy"
                || GameObjectManager::GetInstance().GetGO(entity)->GetTag() == "Dummy")
                && GameObjectManager::GetInstance().GetGO(entity)->GetActive())
            {
                Vec3 enemy_position = ECS::GetInstance().GetComponent<Transform>(entity).position;
                float distance = Vec3Length(enemy_position - skill_position);

                if (distance < skill_range)
                    nearby_enemies.push_back(entity);
            }
        }

        // Return list of nearby enemies
        return nearby_enemies;
    }

    /*!***********************************************************************
    \brief
     Handle soul decay over time, reducing the soul bars periodically.
    *************************************************************************/
    void SoulManager::HandleSoulDecay()
    {
        auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);
        auto& player_animator = ECS::GetInstance().GetComponent<Animation>(player);

        // Decay soul bar over time
        player_soul.soul_decay_timer += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());

        if (player_soul.soul_decay_timer >= player_soul.soul_decay_rate)
        {
            // Decay all soul bars
            for (int i = 0; i < NUM_OF_SOULS; i++)
            {
                // Skip if soul bar is empty
                if (player_soul.soul_bars[i] <= 0.f)
                    continue;

                // Decay soul from the soul bar
                if (player_soul.soul_bars[i] - player_soul.soul_decay_amount <= 0.f)
                {
                    player_soul.soul_bars[i] = 0.f;

                    // Currently in FISH soul, switch to EMPTY soul
                    if (player_soul.current_soul == FISH && player_soul.current_soul == i)
                    {
                        player_soul.current_soul = EMPTY;
                        player_animator.SetAnimationUninterrupted("SwitchBN");
                        GameObjectManager::GetInstance().GetGO(soul)->SetActive(false);
                    }
                    // Currently in WORM soul, switch to EMPTY soul
                    else if (player_soul.current_soul == WORM && player_soul.current_soul == i)
                    {
                        player_soul.current_soul = EMPTY;
                        player_animator.SetAnimationUninterrupted("SwitchRN");
                        GameObjectManager::GetInstance().GetGO(soul)->SetActive(false);
                    }
                }
                else
                    player_soul.soul_bars[i] -= player_soul.soul_decay_amount;
            }

            // Reset soul decay timer
            player_soul.soul_decay_timer = 0.f;
        }
    }

    /*!***********************************************************************
    \brief
     Handle skill effects such as applying abilties, animations and skill cooldown over time.
    *************************************************************************/
    void SoulManager::HandleSkillEffects()
    {
        auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);

        player_soul.skill_timer += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());

        // Check if the skill is ready
        if (!player_soul.skill_ready)
        {
            // If the skill cooldown is done, set skill as ready
            if (player_soul.skill_timer >= player_soul.skill_cooldown)
            {
                // Set the skill to ready
                player_soul.skill_ready = true;

                // Reset the skill timer
                player_soul.skill_timer = 0.f;
            }
        }

        // Apply fish ability effect to nearby enemies while fish ability is active
        if (GameObjectManager::GetInstance().GetGO(fish_ability)->GetActive())
        {
            std::vector<EntityID> nearby_enemies = FindNearbyEnemies();
            for (EntityID enemy : nearby_enemies)
                ApplyFishAbility(enemy);
        }

        // Apply worm ability effect to nearby enemies while worm ability is active
        if (GameObjectManager::GetInstance().GetGO(worm_ability)->GetActive())
        {
            std::vector<EntityID> nearby_enemies = FindNearbyEnemies();
            for (EntityID enemy : nearby_enemies)
                ApplyWormAbility(enemy);
        }

        // Handle the fish spawning and attacking animations, disable fish ability once completed
        auto& fish_animator = ECS::GetInstance().GetComponent<Animation>(fish_ability);
        if (fish_animator.currentClip == "FishAbilitySpawn" && fish_animator.current_frame == 9)
            fish_animator.SetAnimation("FishAbilityAttack");
        if (fish_animator.currentClip == "FishAbilityAttack" && fish_animator.current_frame == 9)
            GameObjectManager::GetInstance().GetGO(fish_ability)->SetActive(false);

        // Handle the worm spawning and attacking animations, disable worm ability once completed
        auto& worm_animator = ECS::GetInstance().GetComponent<Animation>(worm_ability);
        if (worm_animator.currentClip == "WormAbilitySpawn" && worm_animator.current_frame == 9)
            worm_animator.SetAnimation("WormAbilityAttack");
        if (worm_animator.currentClip == "WormAbilityAttack" && worm_animator.current_frame == 9)
            GameObjectManager::GetInstance().GetGO(worm_ability)->SetActive(false);

        // Handle the player casting animation
        auto& player_animator = ECS::GetInstance().GetComponent<Animation>(player);
        if (player_animator.currentClip != "SwitchNB" && player_animator.currentClip != "SwitchNR"
            && player_animator.currentClip != "SwitchBR" && player_animator.currentClip != "SwitchBN"
            && player_animator.currentClip != "SwitchRB" && player_animator.currentClip != "SwitchRN"
            && player_animator.currentClip != "FishSkill" && player_animator.currentClip != "WormSkill")
            player_soul.is_casting = false;
    }

    /*!***********************************************************************
    \brief
     Handle the floating soul which follows the player's position.
    *************************************************************************/
    void SoulManager::HandleFloatingSoul()
    {
        auto& player_transform = ECS::GetInstance().GetComponent<Transform>(player);
        auto& player_sr = ECS::GetInstance().GetComponent<SpriteRender>(player);
        auto& soul_transform = ECS::GetInstance().GetComponent<Transform>(soul);

        // Update floating soul position to the player's position
        if (player_sr.flipX)
            soul_transform.position = Vec3{ player_transform.position.x - player_transform.scale.x * 0.25f, player_transform.position.y + player_transform.scale.y * 0.5f, 0 };
        else
            soul_transform.position = Vec3{ player_transform.position.x + player_transform.scale.x * 0.25f, player_transform.position.y + player_transform.scale.y * 0.5f, 0 };
    }

    /*!***********************************************************************
    \brief
     Handle the movement and harvesting of the enemy soul orbs.
    *************************************************************************/
    void SoulManager::HandleOrbs()
    {
        float min_distance = 30.f; // The minimum distance to harvest the soul
        float frequency = 5.0f; // Controls how fast it oscillates
        float amplitude = 12.0f; // Controls how far it oscillates
        static float orb_time_elapsed = 0.0f; // Track time for oscillation

        for (EntityID orb : m_Entities)
        {
            if (GameObjectManager::GetInstance().GetGO(orb)->GetTag() == "Orb" && GameObjectManager::GetInstance().GetGO(orb)->GetActive())
            {
                auto& orb_position = ECS::GetInstance().GetComponent<Transform>(orb).position;
                auto& soul_orb = ECS::GetInstance().GetComponent<SoulOrb>(orb);

                Vec3 direction = soul_orb.target_position - orb_position;
                float distance = Vec3Length(direction);

                // Harvest and destroy soul orb when its close enough
                if (distance < min_distance)
                {
                    HarvestSoul(soul_orb.orb_type, soul_orb.soul_amount);
                    GameObjectManager::GetInstance().GetGO(orb)->SetActive(false);
                    continue;
                }

                // Normalize direction
                Vec3Normalize(direction, direction);

                // Get perpendicular direction for oscillation
                Vec3 perp_direction = Vec3(-direction.y, direction.x, 0);

                // Apply S-curve movement using a sine wave
                float sine_offset = sin(orb_time_elapsed * frequency) * amplitude;
                Vec3 s_curve_offset = perp_direction * sine_offset; // Apply oscillation perpendicular to movement

                // Move towards the target position
                orb_position += (direction * soul_orb.orb_speed * static_cast<float>(g_FrameRateController.GetFixedDeltaTime())) + s_curve_offset;
            }
        }

        // Increment time for sine wave
        orb_time_elapsed += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
    }

    /*!***********************************************************************
    \brief
     Handle the enemy projectile & boss logic (placeholder).
    *************************************************************************/
    void SoulManager::HandleEnemyProjectile()
    {
        for (auto& entity : m_Entities)
        {
            std::string tag = GameObjectManager::GetInstance().GetGO(entity)->GetTag();

            // Skip if the entity is not active
            if (!GameObjectManager::GetInstance().GetGO(entity)->GetActive())
                continue;

            if (tag == "EnemyProjectile")
            {
                if (GameObjectManager::GetInstance().GetGO(entity)->HasComponent<EnemyBullet>()) // safety check
                {
                    if (GameObjectManager::GetInstance().GetGO(entity)->GetComponent<EnemyBullet>().hit)//if bullet hit detroy
                    {
                        GameObjectManager::GetInstance().GetGO(entity)->SetActive(false);
                        GameObjectManager::GetInstance().DestroyObject(entity);
                        break;
                    }

                    //life time
                    GameObjectManager::GetInstance().GetGO(entity)->GetComponent<EnemyBullet>().lifetime -= static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
                    if (GameObjectManager::GetInstance().GetGO(entity)->GetComponent<EnemyBullet>().lifetime < 0.f)
                    {
                        GameObjectManager::GetInstance().GetGO(entity)->SetActive(false);
                        GameObjectManager::GetInstance().DestroyObject(entity);
                        break;
                    }
                }
            }


            if (tag == "Blob")
            {
                //scale up
                if (GameObjectManager::GetInstance().GetGO(entity)->GetComponent<Transform>().scale.x < 100.f)
                {
                    GameObjectManager::GetInstance().GetGO(entity)->GetComponent<Transform>().scale.x += static_cast<float>(g_FrameRateController.GetFixedDeltaTime()) * 50.f;
                    GameObjectManager::GetInstance().GetGO(entity)->GetComponent<Transform>().scale.y += static_cast<float>(g_FrameRateController.GetFixedDeltaTime()) * 50.f;
                }
                else
                {
                    GameObjectManager::GetInstance().GetGO(entity)->GetComponent<Animation>().SetAnimation("Explode");

                    if (GameObjectManager::GetInstance().GetGO(entity)->GetComponent<Animation>().GetCurrentFrame() == 5)
                    {
                        if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))//safety check
                        {
                            auto audioObj = GameObjectManager::GetInstance().GetGOByTag("AudioManager");

                            if (audioObj->HasComponent<AudioManager>()) //safety check
                            {
                                AudioManager& audio = audioObj->GetComponent<AudioManager>();

                                // Check if the blob spawn audio exists and is not already playing
                                if (audio.GetSFXindex("BlobExplode") != -1)
                                {
                                    if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audio.GetSFXindex("BlobExplode")))
                                    {
                                        audio.PlaySFX(audio.GetSFXindex("BlobExplode"));
                                    }
                                }
                            }

                        }
                    }

                    if (GameObjectManager::GetInstance().GetGO(entity)->HasComponent<Animation>()) //safety check
                    {
                        if (GameObjectManager::GetInstance().GetGO(entity)->GetComponent<Animation>().GetCurrentFrame() == 19)
                        {
                            GameObjectManager::GetInstance().GetGO(entity)->SetActive(false);
                            //spawn monster
                            ECS::GetInstance().GetSystem<BossManager>()->SpawnMonster(GameObjectManager::GetInstance().GetGO(entity)->GetComponent<Transform>().position.x, GameObjectManager::GetInstance().GetGO(entity)->GetComponent<Transform>().position.y);
                            GameObjectManager::GetInstance().DestroyObject(entity);
                            break;
                        }
                    }

                }
            }
        }
    }
}
