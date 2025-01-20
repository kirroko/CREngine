/* Start Header ************************************************************************/
/*!
\file       SoulManager.h
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Jan 15, 2025
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
    class SoulManager
    {
    private:
        /*!***********************************************************************
        \brief
         Maximum amount for a full soul bar.
        *************************************************************************/
        const float MAX_SOUL_BAR = 100.f;

        /*!***********************************************************************
        \brief
         Maximum number of soul charges.
        *************************************************************************/
        const int MAX_SOUL_CHARGES = 3;

    public:
        void Init();
        void Update();

        void HarvestSoul(SoulType soul_type);
        void SwitchSouls();

        void UseSoulAbility();
        void FishAbility();
        void WormAbility();

        void HandleSoulDecay();
    };
}