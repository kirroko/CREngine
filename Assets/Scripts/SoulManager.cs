/* Start Header ************************************************************************/
/*!
\file       SoulManager.cs
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Nov 18, 2024
\brief      This file contains the implementation of the soul harvesting logic.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

using Ukemochi;

public enum SoulType { Empty, Fish, Worm };

public class SoulManager : BaseScript
{
    const float MAX_SOUL_BAR = 100;
    const int MAX_SOUL_CHARGES = 3;

    public SoulType current_soul_type;

    public float fish_soul_bar = 0;
    public float worm_soul_bar = 0;
    public int fish_souls_charges = 0;
    public int worm_souls_charges = 0;

    public float soul_decay_rate = 1f;
    public float soul_decay_timer = 0;

    public void HarvestSoul(SoulType soul_type)
    {
        if (soul_type == SoulType.Fish)
        {
            fish_soul_bar += 15;
            ++fish_souls_charges;
        }
        else if (soul_type == SoulType.Worm)
        {
            worm_soul_bar += 15;
            ++worm_souls_charges;
        }
    }

    public void SwitchSouls()
    {
        if (current_soul_type == SoulType.Fish)
            current_soul_type = SoulType.Worm;
        else
            current_soul_type = SoulType.Fish;
    }

    public void UseSoulAbility()
    {
        if (current_soul_type == SoulType.Fish && fish_souls_charges > 0)
        {
            FishAbility();
            --fish_souls_charges;
        }
        else if (current_soul_type == SoulType.Worm && worm_souls_charges > 0)
        {
            WormAbility();
            --worm_souls_charges;
        }
    }

    void FishAbility()
    {
        // Trigger fish AOE effect
    }

    void WormAbility()
    {
        // Trigger worm web effect
    }
}