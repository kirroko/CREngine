/* Start Header ************************************************************************/
/*!
\file       SoulManager.h
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Mar 27, 2025
\brief      This file contains the declaration of the SoulManager which handles the soul system.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include "../ECS/ECS.h" // for ECS system and components

namespace Ukemochi
{
    const float MAX_SOUL_BAR = 100.f; // Maximum amount for a full soul bar.
    const int MAX_SOUL_CHARGES = 1;   // Maximum number of soul charges.

    class SoulManager : public System
    {
    public:
        EntityID player;       // Keep track of the player ID
        EntityID soul;         // Keep track of the floating soul ID
        EntityID fish_ability; // Keep track of the player's fish ability ID
        EntityID worm_ability; // Keep track of the player's worm ability ID

        /*!***********************************************************************
        \brief
         Initialize the soul manager, finding the player and ability-related entities.
        *************************************************************************/
        void Init();

        /*!***********************************************************************
        \brief
         Update the soul manager, handling player inputs and soul decay.
        *************************************************************************/
        void Update();

        /*!***********************************************************************
        \brief
         Harvest soul into the player's soul bar based on the given soul type and amount.
        \param[in] soul_type
         The type of soul to harvest (e.g., FISH, WORM).
        \param[in] amount
         The amount of soul to harvest.
        *************************************************************************/
        void HarvestSoul(const SoulType soul_type, const float amount);

    private:
        /*!***********************************************************************
        \brief
         Switch between the player's souls (FISH, WORM, or EMPTY) based on available soul bars.
        *************************************************************************/
        void SwitchSouls();

        /*!***********************************************************************
        \brief
         Use the current soul ability, if the player has enough charges.
        *************************************************************************/
        void UseSoulAbility();

        /*!***********************************************************************
        \brief
         Contain the logic for using the fish ability.
         Trigger fish animations, find nearest enemy position and use soul charge.
        *************************************************************************/
        void UseFishAbility();

        /*!***********************************************************************
        \brief
         Contain the logic for using the worm ability.
         Trigger worm animations, find nearest enemy position and use soul charge.
        *************************************************************************/
        void UseWormAbility();

        /*!***********************************************************************
        \brief
         Contain the logic for applying the fish ability.
         Mochi's Fish Ability and Enemy (Enemy takes huge damage).
        *************************************************************************/
        void ApplyFishAbility(const EntityID& enemy);

        /*!***********************************************************************
        \brief
         Contain the logic for applying the worm ability.
         Mochi's Worm Ability and Enemy (Enemy gets trap in the web).
        *************************************************************************/
        void ApplyWormAbility(const EntityID& enemy);

        /*!***********************************************************************
        \brief
         Find the position of the nearest enemy relative to the player.
        \return
         The position of the nearest enemy or the player's position if no enemy exist.
        *************************************************************************/
        Vector3D FindNearestEnemyPosition();

        /*!***********************************************************************
        \brief
         Find a list of nearby enemies within the skill range of the player's current soul ability.
        \return
         A vector of EntityIDs representing the enemies within the ability's range.
        *************************************************************************/
        std::vector<EntityID> FindNearbyEnemies();

        /*!***********************************************************************
        \brief
         Handle soul decay over time, reducing the soul bars periodically.
        *************************************************************************/
        void HandleSoulDecay();

        /*!***********************************************************************
        \brief
         Handle skill effects such as applying abilties, animations and skill cooldown over time.
        *************************************************************************/
        void HandleSkillEffects();

        /*!***********************************************************************
        \brief
         Handle the floating soul which follows the player's position.
        *************************************************************************/
        void HandleFloatingSoul();

        /*!***********************************************************************
        \brief
         Handle the enemy projectile logic (placeholder).
        *************************************************************************/
        void HandleEnemyProjectile();
    };
}