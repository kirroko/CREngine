using System;
using System.Runtime.CompilerServices;
using UkemochiEngine.CoreModule;

public class Player : BaseScript
{
    Transform playerTransform;
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
        if (Input.GetKeyDown(KeyCode.W) || Input.GetKeyDown(KeyCode.Up))
        {
            Debug.Log("Adding force on Y");
            rigi2D.AddForceY(PLAYER_FORCE);
        }
        else if (Input.GetKeyDown(KeyCode.S) || Input.GetKeyDown(KeyCode.Down))
        {
            Debug.Log("Adding force on Y");
            rigi2D.AddForceY(-PLAYER_FORCE);
        }
        else
        {
            rigi2D.RemoveForceY();
        }


        if (Input.GetKeyDown(KeyCode.A) || Input.GetKeyDown(KeyCode.Left))
        {
            Debug.Log("Adding force on X");
            rigi2D.AddForceX(-PLAYER_FORCE);
        }
        else if (Input.GetKeyDown(KeyCode.D) || Input.GetKeyDown(KeyCode.Right))
        {
            Debug.Log("Adding force on X");
            rigi2D.AddForceX(PLAYER_FORCE);
        }
        else
        {
            rigi2D.RemoveForceX();
        }
        
    }
}
