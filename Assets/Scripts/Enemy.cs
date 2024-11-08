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
        direction = new System.Numerics.Vector2 (1, 1);
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

        // Calculate the difference vector
        float dx = playerPosition.x - enemyPosition.x;
        float dy = playerPosition.y - enemyPosition.y;

        // Calculate the squared distance
        float distanceSquared = (float)(Math.Pow(dx, 2) + Math.Pow(dy, 2));
        float combinedRadiusSquared = 200;
        //float combinedRadiusSquared = (playerObj.GetComponent<Transform>().scale.x + enemyTransform.scale.x) *
        //                             (playerObj.GetComponent<Transform>().scale.x + enemyTransform.scale.x);

        //Debug.Log(playerObj.GetComponent<Transform>().position.x.ToString());
        //Debug.Log(enemyTransform.position.x.ToString());
        //Debug.Log(distanceSquared.ToString());

        
        if (enemyStates != EnemyStates.ROAM_STATE)
        {
            System.Numerics.Vector2 newDir = new System.Numerics.Vector2(1 / (playerObj.GetComponent<Transform>().position.x - enemyTransform.position.x), 1 / (playerObj.GetComponent<Transform>().position.y - enemyTransform.position.y));
            direction = 0.95f * direction + 0.05f * newDir;
            direction = new System.Numerics.Vector2(1/direction.X, 1/direction.Y);

            GetComponent<Rigidbody2D>().Velocity = new Ukemochi.Vector2(direction.X, direction.Y);
        }

        switch (enemyStates)
        {
            case EnemyStates.ROAM_STATE:
                //Debug.Log("ROAM");
                GetComponent<Rigidbody2D>().Velocity = new Ukemochi.Vector2(10, 10);
                if (distanceSquared < 100f)
                {
                    enemyStates = EnemyStates.CHASE_STATE;
                }
                break;

            case EnemyStates.CHASE_STATE:
                //Debug.Log("CHASE");
                GetComponent<Rigidbody2D>().Velocity = new Ukemochi.Vector2(75, 75);
                if (distanceSquared < combinedRadiusSquared)
                {
                    Debug.Log("Collide");
                    enemyStates = EnemyStates.ATTACK_STATE;
                }
                if (distanceSquared > 100f)
                {
                    enemyStates = EnemyStates.ROAM_STATE;
                }
                break;

            case EnemyStates.ATTACK_STATE:
                //Debug.Log("ATTACK");
                if (distanceSquared > combinedRadiusSquared)
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
