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
    Rigidbody2D enemyRigi2D;
    Rigidbody2D playerRigi2D;

    Transform playerTran;

    public override void Start()
    {
        enemyStates = EnemyStates.ROAM_STATE;
        enemyRigi2D = GetComponent<Rigidbody2D>();
        var go = GameObject.FindWithTag("Player");
        if (go != null)
        {
            Debug.Log("Found Player!");
            playerTran = go.transform;
            if (playerTran != null)
            {
                Debug.Log("!!! player transform !!!");
            }
        }

        try
        {
            //playerRigi2D = GetComponent<Player>().GetPlayer();
            Console.WriteLine("found player");
        }
        catch (Exception)
        {

            throw;
        }

    }

    public override void Update()
    {
        if (playerTran != null)
        {
            Debug.Log("Player's Position: " + playerTran.position);
            Vector2 distanceSquared = playerTran.position - gameObject.transform.position;
            //float distanceSquaredX = playerRigi2D.gameObject.transform.position.x - enemyRigi2D.gameObject.transform.position.x;
            //float distanceSquaredY = playerRigi2D.gameObject.transform.position.y - enemyRigi2D.gameObject.transform.position.y;
            //float distanceSquared = distanceSquaredX * distanceSquaredX + distanceSquaredY * distanceSquaredY;
            //float combinedRadiusSquared = (playerRigi2D.gameObject.transform.scale.x + enemyRigi2D.gameObject.transform.scale.x);
        }

        switch (enemyStates)
        {
            case EnemyStates.ROAM_STATE:
                //Console.WriteLine("ROAM");
                break;

            case EnemyStates.CHASE_STATE:
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
