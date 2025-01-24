/* Start Header ************************************************************************/
/*!
\file       SoulManager.cpp
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Jan 24, 2025
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
    void SoulManager::Init()
    {
        // Find the player, fish ability and worm abilty entities
        player = fish_ability = worm_ability = static_cast<EntityID>(-1);
        for (auto const& entity : m_Entities)
        {
            if (GameObjectManager::GetInstance().GetGO(entity)->GetTag() == "Player")
            {
                // Player ID is found, set initialize stats
                player = entity;

                auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(entity);
                player_soul.current_soul = EMPTY;

                for (int i = 0; i < NUM_OF_SOULS; i++)
                {
                    player_soul.soul_bars[i] = 0.f;
                    player_soul.soul_charges[i] = 0;
                    player_soul.soul_damage[i] = 50.f;
                }
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

    void SoulManager::Update()
    {
        // Check if player ID is valid
        if (player == -1)
            return;

        // Check if player is alive
        auto& player_data = ECS::GetInstance().GetComponent<Player>(player);
        if (player_data.currentHealth <= 0)
            return;

        // Soul Switch Key Press
        if (Input::IsKeyTriggered(UME_KEY_Q))
            SwitchSouls();

        // Soul Ability Key Press
        if (Input::IsKeyTriggered(UME_KEY_K))
            UseSoulAbility();

        // Decay soul bar over time
        HandleSoulDecay();
    }

    void SoulManager::HarvestSoul(SoulType soul_type, float amount)
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
    }

    void SoulManager::SwitchSouls()
    {
        auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);

        // Currently in EMPTY soul, switch to FISH or WORM souls if available
        if (player_soul.current_soul == SoulType::EMPTY)
        {
            if (player_soul.soul_bars[FISH] > 0)
                player_soul.current_soul = FISH;
            else if (player_soul.soul_bars[WORM] > 0)
                player_soul.current_soul = WORM;
        }
        // Currently in FISH soul, switch to WORM soul if available else EMPTY soul
        else if (player_soul.current_soul == FISH)
        {
            if (player_soul.soul_bars[WORM] > 0)
                player_soul.current_soul = WORM;
            else
                player_soul.current_soul = EMPTY;
        }
        // Currently in WORM soul, switch to FISH soul if available else EMPTY soul
        else if (player_soul.current_soul == WORM)
        {
            if (player_soul.soul_bars[FISH] > 0)
                player_soul.current_soul = FISH;
            else
                player_soul.current_soul = EMPTY;
        }
    }

    void SoulManager::UseSoulAbility()
    {
        auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);

        // Use the FISH ability if there are enough FISH charges
        if (player_soul.current_soul == FISH && player_soul.soul_charges[FISH] > 0)
        {
            FishAbility();
            --player_soul.soul_charges[player_soul.current_soul];
        }
        // Use the WORM ability if there are enough WORM charges
        else if (player_soul.current_soul == WORM && player_soul.soul_charges[WORM] > 0)
        {
            WormAbility();
            --player_soul.soul_charges[player_soul.current_soul];
        }
    }

    void SoulManager::FishAbility()
    {
        // Trigger fish AOE effect
        GameObjectManager::GetInstance().GetGO(fish_ability)->SetActive(true);
    }

    void SoulManager::WormAbility()
    {
        // Trigger worm web effect
        GameObjectManager::GetInstance().GetGO(worm_ability)->SetActive(true);
    }

    void SoulManager::HandleSoulDecay()
    {
        // Update the soul system based on the number of steps
        for (int step = 0; step < g_FrameRateController.GetCurrentNumberOfSteps(); ++step)
        {
            auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);

            // Decay non-active soul bar over time
            player_soul.soul_decay_timer += g_FrameRateController.GetFixedDeltaTime();

            if (player_soul.soul_decay_timer >= player_soul.soul_decay_rate)
            {
                for (int i = 0; i < NUM_OF_SOULS; i++)
                {
                    // Decay non-active soul bars
                    if (player_soul.current_soul != i && player_soul.soul_bars[i] > player_soul.soul_decay_amount)
                        player_soul.soul_bars[i] -= player_soul.soul_decay_amount;
                }

                // Reset soul decay timer
                player_soul.soul_decay_timer = 0;
            }
        }
    }
}