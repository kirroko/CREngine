/* Start Header ************************************************************************/
/*!
\file       SoulManager.h
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Jan 24, 2025
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
    class SoulManager : public System
    {
    private:
        const float MAX_SOUL_BAR = 5.f; // Maximum amount for a full soul bar.
        const int MAX_SOUL_CHARGES = 1; // Maximum number of soul charges.

        EntityID player;       // Keep track of the player ID
        EntityID fish_ability; // Keep track of the player's fish ID
        EntityID worm_ability; // Keep track of the player's worm ID

    public:
        void Init();
        void Update();

        void HarvestSoul(SoulType soul_type, float amount);
        void SwitchSouls();

        void UseSoulAbility();
        void FishAbility();
        void WormAbility();

        void HandleSoulDecay();
    };
}