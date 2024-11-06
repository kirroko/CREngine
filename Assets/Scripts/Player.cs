using System;
using System.Runtime.CompilerServices;
using UkemochiEngine.CoreModule;

public class Player : BaseScript
{
    Rigidbody2D rigi2D;

    const float PLAYER_FORCE = 750.0f;

    public override void Start()
    {
        Debug.Log("Hello world! Player script start is called");
        rigi2D = GetComponent<Rigidbody2D>();
    }

    public override void Update()
    {
        //Debug.Log("Hallo, Update");
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
        else if(Input.GetKey(KeyCode.T))
            rigi2D.AddTorque(PLAYER_FORCE);
        else
            rigi2D.RemoveTorque();
    }

    public Rigidbody2D GetPlayer()
    {
        return GetComponent<Rigidbody2D>();
    }
}
