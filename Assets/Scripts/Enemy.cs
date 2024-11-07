using System;
using System.Runtime.CompilerServices;
using UkemochiEngine.CoreModule;

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
        float distanceX = playerObj.GetComponent<Transform>().Position.x - enemyTransform.Position.x;
        float distanceY = playerObj.GetComponent<Transform>().Position.y - enemyTransform.Position.y;
        float distanceSquared = (distanceX * distanceX + distanceY * distanceY);
        float combinedRadiusSquared = (playerObj.GetComponent<Transform>().Scale.x + enemyTransform.Scale.x);

        Debug.Log(playerObj.GetComponent<Transform>().Position.x.ToString());
        Debug.Log(enemyTransform.Position.x.ToString());

        switch (enemyStates)
        {
            case EnemyStates.ROAM_STATE:
                GetComponent<Rigidbody2D>().Velocity = new Vector2(0, 0);
                if (distanceSquared < 100F)
                {
                    enemyStates = EnemyStates.CHASE_STATE;
                }
                break;

            case EnemyStates.CHASE_STATE:
                GetComponent<Rigidbody2D>().Velocity = new Vector2(50, 50);
                break;

            case EnemyStates.ATTACK_STATE:
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
