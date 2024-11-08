/* Start Header
*****************************************************************/
/*!
\file       Enemy.cs
\author     Tan Si Han, t.sihan, 2301264
\par        email: t.sihan\@digipen.edu
\date       Nov 8, 2024
\brief      Basic Enemy StateMachine in Csharp

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
using System;
using System.Runtime.CompilerServices;
using Ukemochi;
using System.Numerics;

public class Enemy : BaseScript
{
    enum EnemyStates
    {
        ROAM_STATE,
        CHASE_STATE,
        ATTACK_STATE,
        DEAD_STATE
    }
    EnemyStates enemyStates;
    Transform enemyTransform;
    GameObject playerObj = null;

    private System.Numerics.Vector2 direction;
    private float movementSpeed = 1.0f; // Adjust movement speed
    //private float smoothingFactor = 0.05f;
    float currentDirX = 0.0f; // Initialize and store outside of loop/scope for persistent direction state
    float currentDirY = 0.0f;

    Transform playerTran;

    public override void Start()
    {
        direction = new System.Numerics.Vector2 (0, 0);
        enemyStates = EnemyStates.ROAM_STATE;
        enemyTransform = GetComponent<Transform>();
        playerObj = GameObject.FindWithTag("Player");

    }

    public override void Update()
    {
        if (playerObj.GetComponent<Transform>() == null)
        {
            Debug.Log("PLAYER NULL " + playerObj.ToString());
        }
        if (enemyTransform == null)
        {
            Debug.Log("ENEMY NULL");
        }

        //Console.WriteLine("HIHIIHIHIH found player");
        Ukemochi.Vector2 playerPosition = playerObj.GetComponent<Transform>().position;
        Ukemochi.Vector2 enemyPosition = enemyTransform.position;
        //Debug.Log("PlayerX " + playerPosition.x.ToString() + " : " + "PlayerY" + playerPosition.y.ToString());
        //Debug.Log("EnemyX " + enemyPosition.x.ToString() + " : " + "EnemyY" + enemyPosition.y.ToString());

        // Calculate the difference vector
        float dx = playerPosition.x - enemyPosition.x;
        float dy = playerPosition.y - enemyPosition.y;
        // Calculate the squared distance
        float distanceSquared = (float)(Math.Pow(dx, 2) + Math.Pow(dy, 2));
        float combinedRadiusSquared = 200;
        //Debug.Log("Diff " + Math.Sqrt(distanceSquared));
        //float combinedRadiusSquared = (playerObj.GetComponent<Transform>().scale.x + enemyTransform.scale.x) *
        //                             (playerObj.GetComponent<Transform>().scale.x + enemyTransform.scale.x);

        //Debug.Log(playerObj.GetComponent<Transform>().position.x.ToString());
        //Debug.Log(enemyTransform.position.x.ToString());
        //Debug.Log(distanceSquared.ToString());

        
        if (enemyStates != EnemyStates.ROAM_STATE && enemyStates != EnemyStates.ATTACK_STATE)
        {
            System.Numerics.Vector2 directionToPlayer = new System.Numerics.Vector2(
                playerObj.GetComponent<Transform>().position.x - enemyTransform.position.x,
                playerObj.GetComponent<Transform>().position.y - enemyTransform.position.y
            );

            // Normalize the direction vector
            float magnitude = (float)Math.Sqrt(directionToPlayer.X * directionToPlayer.X + directionToPlayer.Y * directionToPlayer.Y);
            if (magnitude > 0)
            {
                directionToPlayer /= magnitude; // Normalize to unit length
                direction = 0.95f * direction + 0.05f * directionToPlayer;

                // Set velocity based on direction scaled by speed
                float speed = 150; // Adjust speed value as necessary
                GetComponent<Rigidbody2D>().Velocity = new Ukemochi.Vector2(direction.X * speed, direction.Y * speed);
                //Debug.Log("Velocity: " + GetComponent<Rigidbody2D>().Velocity);
            }
            else
            {
                GetComponent<Rigidbody2D>().Velocity = Ukemochi.Vector2.Zero;
            }
            //System.Numerics.Vector2 newDir = new System.Numerics.Vector2(1 / (playerObj.GetComponent<Transform>().position.x - enemyTransform.position.x), 1 / (playerObj.GetComponent<Transform>().position.y - enemyTransform.position.y));
            //direction = 0.95f * direction + 0.05f * newDir;
            //direction = new System.Numerics.Vector2(1/direction.X, 1/direction.Y);

            //GetComponent<Rigidbody2D>().Velocity = new Ukemochi.Vector2(direction.X, direction.Y);
            Debug.Log("Vel " + GetComponent<Rigidbody2D>().Velocity.ToString());
        }

        switch (enemyStates)
        {
            case EnemyStates.ROAM_STATE:
                //Debug.Log("ROAM : ");
                //GetComponent<Rigidbody2D>().Velocity = new Ukemochi.Vector2(50, 50);
                if (Math.Sqrt(distanceSquared) < combinedRadiusSquared + 50f)
                {
                    enemyStates = EnemyStates.CHASE_STATE;
                }
                break;

            case EnemyStates.CHASE_STATE:
               //Debug.Log("CHASE");
                //GetComponent<Rigidbody2D>().Velocity = new Ukemochi.Vector2(75, 75);
                if (Math.Sqrt(distanceSquared) < combinedRadiusSquared - 50f)
                {
                  //  Debug.Log("Collide");
                    enemyStates = EnemyStates.ATTACK_STATE;
                }
                if (Math.Sqrt(distanceSquared) > combinedRadiusSquared +50f)
                {
                    enemyStates = EnemyStates.ROAM_STATE;
                }
                break;

            case EnemyStates.ATTACK_STATE:
                //Debug.Log("ATTACK");
                GetComponent<Rigidbody2D>().Velocity = new Ukemochi.Vector2(0, 0);
                if (Math.Sqrt(distanceSquared) > combinedRadiusSquared)
                {
                    enemyStates = EnemyStates.CHASE_STATE;
                }
                break;

            case EnemyStates.DEAD_STATE:
                break;

            default:
                break;

        }
    }

    public override void FixedUpdate()
    {
    }
}
