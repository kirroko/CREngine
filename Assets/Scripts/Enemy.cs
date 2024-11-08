using System;
using System.Runtime.CompilerServices;
using Ukemochi;

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

    //private float directionX;
   // private float directionY;
    private float movementSpeed = 1.0f; // Adjust movement speed
    //private float smoothingFactor = 0.05f;
    float currentDirX = 0.0f; // Initialize and store outside of loop/scope for persistent direction state
    float currentDirY = 0.0f;

    Transform playerTran;

    public override void Start()
    {
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
        float distanceX = playerObj.GetComponent<Transform>().position.x - enemyTransform.position.x;
        float distanceY = playerObj.GetComponent<Transform>().position.y - enemyTransform.position.y;
        float distanceSquared = (distanceX + distanceY)* (distanceX + distanceY);
        float combinedRadiusSquared = 200*200;
        //float combinedRadiusSquared = (playerObj.GetComponent<Transform>().scale.x + enemyTransform.scale.x) *
        //                             (playerObj.GetComponent<Transform>().scale.x + enemyTransform.scale.x);

        //Debug.Log(playerObj.GetComponent<Transform>().position.x.ToString());
        //Debug.Log(enemyTransform.position.x.ToString());
        //Debug.Log(distanceSquared.ToString());
        
        if (enemyStates != EnemyStates.ROAM_STATE)
        {
            // Normalize the direction vector (approximation without sqrt)
            if (distanceSquared > 0.0f)
            {
                float inverseMagnitudeApprox = 1.0f / (1.0f + 0.5f * distanceSquared);
                float newDirX = distanceX * inverseMagnitudeApprox;
                float newDirY = distanceY * inverseMagnitudeApprox;

                // Variables to represent direction
                currentDirX = 0.0f; // Initialize and store outside of loop/scope for persistent direction state
                currentDirY = 0.0f;

                // Smooth the direction
                currentDirX = 0.95f * currentDirX + 0.05f * newDirX;
                currentDirY = 0.95f * currentDirY + 0.05f * newDirY;

                // Normalize the direction vector again after smoothing (approximation)
                float dirMagnitudeSquared = currentDirX * currentDirX + currentDirY * currentDirY;
                if (dirMagnitudeSquared > 0.0f)
                {
                    float inverseDirMagnitudeApprox = 1.0f / (1.0f + 0.5f * dirMagnitudeSquared);
                    currentDirX *= inverseDirMagnitudeApprox;
                    currentDirY *= inverseDirMagnitudeApprox;
                }

                // Set velocity and update position based on direction and movement speed
                float velocityX = currentDirX * movementSpeed;
                float velocityY = currentDirY * movementSpeed;

                GetComponent<Rigidbody2D>().Velocity = new Vector2(velocityX, velocityY);
            }
        }

        switch (enemyStates)
        {
            case EnemyStates.ROAM_STATE:
                //Debug.Log("ROAM");
                GetComponent<Rigidbody2D>().Velocity = new Vector2(10, 10);
                if (distanceSquared < 100f)
                {
                    enemyStates = EnemyStates.CHASE_STATE;
                }
                break;

            case EnemyStates.CHASE_STATE:
                //Debug.Log("CHASE");
                GetComponent<Rigidbody2D>().Velocity = new Vector2(75, 75);
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
