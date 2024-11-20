/* Start Header ************************************************************************/
/*!
\file       PlayerCombat.cs
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Nov 18, 2024
\brief      This file contains the implementation of the player combat logic.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

using Ukemochi;

public class PlayerCombat
{
    // Mochi's Combat
    const int MAX_COMBO_HITS = 3;
    public int current_combo_hits = 0;
    public float attack_cooldown = 0.5f;
    public float attack_timer = 0;
    public bool can_attack = true;

    public SoulManager soul_manager;

    public void HandleCombat()
    {
        // Click left mouse button for basic attacks
        if (Input.GetMouseButton(KeyCode.Mouse0) && can_attack)
        {
            // Slash
            BasicAttack();
        }

        // Click right mouse button for soul abilities
        if (Input.GetMouseButton(KeyCode.Mouse1))
        {
            // Use soul ability
            soul_manager.UseSoulAbility();
        }

        // Press Q to switch souls
        if (Input.GetKey(KeyCode.Q))
        {
            // Switch souls
            soul_manager.SwitchSouls();
        }

        if (!can_attack) // Wait for attack cooldown
        {
            //attack_timer += dt;
            if (attack_timer >= attack_cooldown)
            {
                attack_timer = 0;
                current_combo_hits = 0;
                can_attack = true;
            }
        }
    }

    private void BasicAttack()
    {
        can_attack = false;
        ++current_combo_hits;

        if (current_combo_hits < MAX_COMBO_HITS)
        {
            // Perform slash attack
            //enemy
            //if(other.gameObject.tag == "Enemy")
            //other.gameObject.GetComponent<Enemy>().TakeDamage();
            //soul_manager.HarvestSoul(enemy.SoulType);
        }
        else
        {
            // Perform final knockback attack
            //enemy
            current_combo_hits = 0;
        }
    }
}