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
    Rigidbody2D enemyRigi2D;
    Rigidbody2D playerRigi2D;

    public override void Start()
    {
        enemyStates = EnemyStates.ROAM_STATE;
        enemyRigi2D = GetComponent<Rigidbody2D>();
        playerRigi2D = null;
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
        if (playerRigi2D != null)
        {
            float distanceSquaredX = playerRigi2D.gameObject.transform.Position.x - enemyRigi2D.gameObject.transform.Position.x;
            float distanceSquaredY = playerRigi2D.gameObject.transform.Position.y - enemyRigi2D.gameObject.transform.Position.y;
            float distanceSquared = distanceSquaredX * distanceSquaredX + distanceSquaredY * distanceSquaredY;
            float combinedRadiusSquared = (playerRigi2D.gameObject.transform.Scale.x + enemyRigi2D.gameObject.transform.Scale.x);
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
