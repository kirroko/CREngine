/* Start Header ************************************************************************/
/*!
\file       SoulManager.cpp
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Feb 02, 2025
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

namespace Ukemochi
{
    /*!***********************************************************************
    \brief
     Initialize the soul manager, finding the player and ability-related entities.
    *************************************************************************/
    void SoulManager::Init()
    {
        // Find the player, fish ability and worm abilty entities
        player = fish_ability = worm_ability = static_cast<EntityID>(-1);
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
        // Check if player, floating soul, fish and worm IDs is valid
        if (player == -1 || soul == -1 || fish_ability == -1 || worm_ability == -1)
            return;

        // Check if player is alive
        auto& player_data = ECS::GetInstance().GetComponent<Player>(player);
        if (player_data.currentHealth <= 0)
        {
            // If player is dead, set soul to EMPTY and disable floating soul
            ECS::GetInstance().GetComponent<PlayerSoul>(player).current_soul = EMPTY;
            GameObjectManager::GetInstance().GetGO(soul)->SetActive(false);
            return;
        }

        // Soul Switch Key Press
        if (Input::IsKeyTriggered(UME_KEY_Q))
            SwitchSouls();

        // Soul Ability Key Press
        if (Input::IsKeyTriggered(UME_KEY_F))
            UseSoulAbility();

        // Update the soul system based on the number of steps
        for (int step = 0; step < g_FrameRateController.GetCurrentNumberOfSteps(); ++step)
        {
            // Handle soul bar decay over time
            //HandleSoulDecay();

            // Handle skill effects over time
            HandleSkillEffects();

            // Handle floating soul
            HandleFloatingSoul();
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
            ++player_soul.soul_charges[soul_type];

        UME_ENGINE_TRACE("Soul Harvest: {0}", static_cast<int>(soul_type));
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

        UME_ENGINE_TRACE("OMG HE PRESSED Q!");

        // Currently in EMPTY soul, switch to FISH or WORM souls if available
        if (player_soul.current_soul == SoulType::EMPTY)
        {
            if (player_soul.soul_bars[FISH] > 0)
            {
                player_soul.current_soul = FISH;
                player_animator.SetAnimationUninterrupted("SwitchNB");
                soul_animator.SetAnimation("BlueFlame");
                GameObjectManager::GetInstance().GetGO(soul)->SetActive(true);
            }
            else if (player_soul.soul_bars[WORM] > 0)
            {
                player_soul.current_soul = WORM;
                player_animator.SetAnimationUninterrupted("SwitchNR");
                soul_animator.SetAnimation("RedFlame");
                GameObjectManager::GetInstance().GetGO(soul)->SetActive(true);
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
                GameObjectManager::GetInstance().GetGO(soul)->SetActive(true);
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
                GameObjectManager::GetInstance().GetGO(soul)->SetActive(true);
            }
            else
            {
                player_soul.current_soul = EMPTY;
                player_animator.SetAnimationUninterrupted("SwitchRN");
                GameObjectManager::GetInstance().GetGO(soul)->SetActive(false);
            }
        }

        UME_ENGINE_TRACE("Soul Switch: {0}", static_cast<int>(player_soul.current_soul));
    }

    /*!***********************************************************************
    \brief
     Use the current soul ability, if the player has enough charges.
    *************************************************************************/
    void SoulManager::UseSoulAbility()
    {
        auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);

        // Check if the skill is ready
        if (!player_soul.skill_ready)
            return;

        // Use the FISH ability if there are enough FISH charges
        if (player_soul.current_soul == FISH && player_soul.soul_charges[FISH] > 0)
            FishAbility();
        // Use the WORM ability if there are enough WORM charges
        else if (player_soul.current_soul == WORM && player_soul.soul_charges[WORM] > 0)
            WormAbility();
    }

    /*!***********************************************************************
    \brief
     Contain the logic for the fish ability.
    *************************************************************************/
    void SoulManager::FishAbility()
    {
        // Trigger player fish animation
        auto& player_animator = ECS::GetInstance().GetComponent<Animation>(player);
        player_animator.SetAnimationUninterrupted("FishSkill");

        // Trigger fish AOE effect at the nearest enemy
        auto& fish_transform = ECS::GetInstance().GetComponent<Transform>(fish_ability);
        fish_transform.position = FindNearestEnemyPosition();

        // Trigger fish animation and enable GO
        auto& fish_animator = ECS::GetInstance().GetComponent<Animation>(fish_ability);
        fish_animator.SetAnimation("FishAbilitySpawn");
        //fish_animator.SetAnimation("FishAbilityAttack");
        GameObjectManager::GetInstance().GetGO(fish_ability)->SetActive(true);

        // Decrease soul charge and reset skill stats
        auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);
        --player_soul.soul_charges[player_soul.current_soul];
        player_soul.skill_ready = false;
        player_soul.skill_timer = 0.f;

        UME_ENGINE_TRACE("Soul Ability: FISH");
    }

    /*!***********************************************************************
    \brief
     Contain the logic for the worm ability.
    *************************************************************************/
    void SoulManager::WormAbility()
    {
        // Trigger player worm animation
        auto& player_animator = ECS::GetInstance().GetComponent<Animation>(player);
        player_animator.SetAnimationUninterrupted("WormSkill");

        // Trigger worm web effect at the nearest enemy
        auto& worm_transform = ECS::GetInstance().GetComponent<Transform>(worm_ability);
        worm_transform.position = FindNearestEnemyPosition();
        GameObjectManager::GetInstance().GetGO(worm_ability)->SetActive(true);

        // Trigger worm animation and enable GO
        auto& worm_animator = ECS::GetInstance().GetComponent<Animation>(worm_ability);
        worm_animator.SetAnimation("WormAbilitySpawn");
        //worm_animator.SetAnimation("WormAbilityAttack");
        GameObjectManager::GetInstance().GetGO(worm_ability)->SetActive(true);

        // Decrease soul charge and reset skill stats
        auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);
        --player_soul.soul_charges[player_soul.current_soul];
        player_soul.skill_ready = false;
        player_soul.skill_timer = 0.f;

        UME_ENGINE_TRACE("Soul Ability: WORM");
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
            if (GameObjectManager::GetInstance().GetGO(entity)->GetTag() == "Enemy")
            {
                Vec3 enemy_position = ECS::GetInstance().GetComponent<Transform>(entity).position;
                float distance = Vec3Length(enemy_position - player_position);

                if (distance < min_distance)
                {
                    min_distance = distance;
                    nearest_enemy_position = enemy_position;
                }
            }
        }

        // Return the nearest enemy position
        return nearest_enemy_position;
    }

    /*!***********************************************************************
    \brief
     Handle soul decay over time, reducing the non-active soul bars periodically.
    *************************************************************************/
    void SoulManager::HandleSoulDecay()
    {
        auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);

        // Decay non-active soul bar over time
        player_soul.soul_decay_timer += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());

        if (player_soul.soul_decay_timer >= player_soul.soul_decay_rate)
        {
            for (int i = 0; i < NUM_OF_SOULS; i++)
            {
                // Decay non-active soul bars
                if (player_soul.current_soul != i && player_soul.soul_bars[i] > player_soul.soul_decay_amount)
                    player_soul.soul_bars[i] -= player_soul.soul_decay_amount;
            }

            // Reset soul decay timer
            player_soul.soul_decay_timer = 0.f;
        }
    }

    /*!***********************************************************************
    \brief
     Handle skill effects such as skill duration and cooldown over time.
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
        else
        {
            // If the skill duration has expired, disable the ability
            if (player_soul.skill_timer > player_soul.skill_duration)
            {
                // Disable the skill ability
                if (player_soul.current_soul == FISH)
                    GameObjectManager::GetInstance().GetGO(fish_ability)->SetActive(false);
                else if (player_soul.current_soul == WORM)
                    GameObjectManager::GetInstance().GetGO(worm_ability)->SetActive(false);

                // Reset the skill timer
                player_soul.skill_timer = 0.f;
            }
        }
    }

    /*!***********************************************************************
    \brief
     Handle the floating soul which follows the player's position.
    *************************************************************************/
    void SoulManager::HandleFloatingSoul()
    {
        auto& player_transform = ECS::GetInstance().GetComponent<Transform>(player);
        auto& soul_transform = ECS::GetInstance().GetComponent<Transform>(soul);

        // Update floating soul position to the player's position
        soul_transform.position = Vec3{ player_transform.position.x + soul_transform.scale.x, player_transform.position.y + soul_transform.scale.y, 0 };
    }
}