/* Start Header ************************************************************************/
/*!
\file       SoulManager.cpp
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Jan 15, 2025
\brief      This file contains the definition of the SoulManager which handles the soul system.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "SoulManager.h" // for forward declaration

namespace Ukemochi
{
    void SoulManager::Init()
    {
    }

    void SoulManager::Update()
    {
        HandleSoulDecay();
    }

    void SoulManager::HarvestSoul(SoulType soul_type)
    {
        //if (soul_type == SoulType::Fish)
        //{
        //    fish_soul_bar += 15;
        //    ++fish_souls_charges;
        //}
        //else if (soul_type == SoulType::Worm)
        //{
        //    worm_soul_bar += 15;
        //    ++worm_souls_charges;
        //}
    }

    void SoulManager::SwitchSouls()
    {
        //if (current_soul_type == SoulType::Fish)
        //    current_soul_type = SoulType::Worm;
        //else
        //    current_soul_type = SoulType::Fish;
    }

    void SoulManager::UseSoulAbility()
    {
        //if (current_soul_type == SoulType::Fish && fish_souls_charges > 0)
        //{
        //    FishAbility();
        //    --fish_souls_charges;
        //}
        //else if (current_soul_type == SoulType::Worm && worm_souls_charges > 0)
        //{
        //    WormAbility();
        //    --worm_souls_charges;
        //}
    }

    void SoulManager::FishAbility()
    {
        // Trigger fish AOE effect
    }

    void SoulManager::WormAbility()
    {
        // Trigger worm web effect
    }

    void SoulManager::HandleSoulDecay()
    {
        ////soul_decay_rate += dt;
        //if (soul_decay_timer >= soul_decay_rate)
        //{
        //    soul_decay_timer = 0;

        //    if (fish_souls_charges > 0)
        //        --fish_souls_charges;

        //    if (worm_souls_charges > 0)
        //        --worm_souls_charges;
        //}
    }
}