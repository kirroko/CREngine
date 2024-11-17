/* Start Header ************************************************************************/
/*!
\file       Player.cs
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date       Nov 18, 2024
\brief      This file contains the implementation of the player logic.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

using System;
using System.Runtime.CompilerServices;
using Ukemochi;

public class Player : BaseScript
{
    Rigidbody2D rigi2D;
    GameObject enemy;

    const float PLAYER_FORCE = 1500F;

    // Mochi's Stats
    const float MAX_HEALTH = 100;
    public float current_health = 100;
    public bool is_dead = false;

    public PlayerCombat player_combat;

    public override void Start()
    {
        rigi2D = GetComponent<Rigidbody2D>();
        if (rigi2D == null)
            Debug.LogWarning("Player's Rigi2D is missing!");
        
        enemy = GameObject.FindWithTag("Enemy");
    }

    public override void Update()
    {
        if (rigi2D == null)
            return;

        HandleMovement();
        player_combat.HandleCombat();
    }

    public override void OnCollisionEnter2D()
    {
        //enemy.GetComponent<Enemy>().TakeDamage(10);
        //player_combat.soul_manager.HarvestSoul(enemy.GetComponent<Enemy>().soul_type);
    }

    private void HandleMovement()
    {
        if (Input.GetKey(KeyCode.W))
            rigi2D.AddForceY(PLAYER_FORCE);
        else if (Input.GetKey(KeyCode.S))
            rigi2D.AddForceY(-PLAYER_FORCE);
        else
            rigi2D.RemoveForceY();

        if (Input.GetKey(KeyCode.A))
            rigi2D.AddForceX(-PLAYER_FORCE);
        else if (Input.GetKey(KeyCode.D))
            rigi2D.AddForceX(PLAYER_FORCE);
        else
            rigi2D.RemoveForceX();

        if (Input.GetKey(KeyCode.R))
            rigi2D.AddTorque(-PLAYER_FORCE);
        else if (Input.GetKey(KeyCode.T))
            rigi2D.AddTorque(PLAYER_FORCE);
        else
            rigi2D.RemoveTorque();
    }
}
