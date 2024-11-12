using System;
using System.Runtime.CompilerServices;
using Ukemochi;

public class Player : BaseScript
{
    Rigidbody2D rigi2D;
    GameObject enemy;

    const float PLAYER_FORCE = 1500F;

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
