#include "PreCompile.h"
//
//
//for (int step = 0; step < g_FrameRateController.GetCurrentNumberOfSteps(); ++step)
//{
//
//    // Iterate through the enemyObjects list
//    for (auto it = enemyObjects.begin(); it != enemyObjects.end();)
//    {
////NO NEED 
//        //if there no nearest obj, find neareast obj
//        if (enemycomponent.nearestObj == -1)
//        {
//            enemycomponent.nearestObj = FindNearestObject(object);
//        }
//
//        //if (enemycomponent.state != enemycomponent.ATTACK)
//        //{
//        //    bool isMoving = (std::abs(enemyphysic.force.x) > 0.1f || std::abs(enemyphysic.force.y) > 0.1f);
//        //}
//
//        if (playerObj != nullptr)
//        {
//            if (enemycomponent.ReachedTarget(enemytransform.position.x, enemytransform.position.y,
//                playerObj->GetComponent<Transform>().position.x,
//                playerObj->GetComponent<Transform>().position.y, 350.f) == true && enemycomponent.state == enemycomponent.ROAM)
//            {
//                //std::cout << object->GetInstanceID() << " Chase" << std::endl;
//                enemycomponent.state = enemycomponent.CHASE;
//            }
//        }
//
//        //ENEMY COLLISION RESPONSE
//        if (enemycomponent.isCollide)
//        {
//            // Start the timer for 1 second if not already running
//            if (enemycomponent.timeSinceTargetReached < 1.0f) {
//                enemycomponent.timeSinceTargetReached += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
//            }
//            else {
//
//                // Timer has reached 1 second, perform the object updates
//                auto* collidedObj = GameObjectManager::GetInstance().GetGO(enemycomponent.collideObj);
//                if (collidedObj == nullptr)
//                    break;
//                //WHEN COLLIDE TO WALL
//                if (collidedObj->GetTag() == "Boundary")
//                {
//                    enemyphysic.force.x = -enemycomponent.dirX * enemycomponent.speed;
//                    enemyphysic.force.y = -enemycomponent.dirY * enemycomponent.speed;
//
//                    if (enemycomponent.timeSinceTargetReached > 3.0f)
//                    {
//                        enemycomponent.prevObject2 = enemycomponent.prevObject;
//                        enemycomponent.prevObject = enemycomponent.nearestObj;
//                        enemycomponent.nearestObj = -1;
//                        enemycomponent.collideObj = -1;
//
//                        if (enemycomponent.nearestObj == enemycomponent.prevObject2 && enemycomponent.nearestObj == enemycomponent.prevObject)
//                        {
//                            enemycomponent.nearestObj = FindNearestObject(object);
//                        }
//                        else
//                        {
//                            enemycomponent.isCollide = false;
//                            enemycomponent.timeSinceTargetReached = 0.f;
//                        }
//                    }
//
//                }
//                //ENEMY TO COLLIDE OBJ
//                else
//                {
//                    auto& collidedTransform = ECS::GetInstance().GetComponent<Transform>(enemycomponent.collideObj);
//                    Vec2 awayDir;
//                    Vec2Normalize(awayDir, Vec2(enemytransform.position.x - collidedTransform.position.x,
//                        enemytransform.position.y - collidedTransform.position.y));
//
//                    enemycomponent.dirX = awayDir.x;
//                    enemycomponent.dirY = awayDir.y;
//                    enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
//                    enemyphysic.force.y = enemycomponent.dirY * enemycomponent.speed;
//
//                    if (IsEnemyAwayFromObject(object, GameObjectManager::GetInstance().GetGO(enemycomponent.collideObj), 300.f) && enemycomponent.state == enemycomponent.MOVE)
//                    {
//                        enemycomponent.prevObject2 = enemycomponent.prevObject;
//                        enemycomponent.prevObject = enemycomponent.nearestObj;
//                        enemycomponent.nearestObj = -1;
//                        enemycomponent.collideObj = -1;
//
//                        if (enemycomponent.nearestObj == enemycomponent.prevObject2 && enemycomponent.nearestObj == enemycomponent.prevObject)
//                        {
//                            enemycomponent.nearestObj = FindNearestObject(object);
//                        }
//                        else
//                        {
//                            enemycomponent.isCollide = false;
//                            enemycomponent.timeSinceTargetReached = 0.f;
//                        }
//
//                    }
//                }
//                //enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
//                //enemyphysic.force.y = -enemycomponent.dirY * enemycomponent.speed;
//            }
//            ++it;
//            continue;
//        }
//
//
//
//        // Handle other states
//        switch (enemycomponent.state)
//        {
//        case Enemy::ROAM:
//            if (object->GetComponent<Animation>().currentClip != "Walk")
//            {
//                object->GetComponent<Animation>().SetAnimation("Walk");
//            }
//            // Compute the direction vector to the target waypoint
//            enemycomponent.dirX = enemycomponent.targetX - enemytransform.position.x;
//            enemycomponent.dirY = enemycomponent.targetY - enemytransform.position.y;
//
//            // Calculate the distance to normalize the direction
//            enemycomponent.magnitude = std::sqrt(enemycomponent.dirX * enemycomponent.dirX + enemycomponent.dirY * enemycomponent.dirY);
//
//            if (enemycomponent.magnitude > 0.0f) // Avoid division by zero
//            {
//                // Normalize the direction vector
//                enemycomponent.dirX /= enemycomponent.magnitude;
//                enemycomponent.dirY /= enemycomponent.magnitude;
//
//                // Scale the normalized direction by the enemy's speed
//                enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
//                enemyphysic.force.y = enemycomponent.dirY * enemycomponent.speed;
//
//                if (enemycomponent.ReachedTarget(enemyphysic.position.x, enemyphysic.position.y, enemycomponent.targetX, enemycomponent.targetY, 20.f))
//                {
//                    enemyphysic.velocity.x = 0.0f;
//                    enemyphysic.velocity.y = 0.0f;
//                    enemyphysic.force.x = enemyphysic.force.y = 0.f;
//                    // Start the timer for 1 second if not already running
//                    if (enemycomponent.timeSinceTargetReached < 1.0f) {
//                        enemycomponent.timeSinceTargetReached += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
//                    }
//                    else {
//                        // Timer has reached 1 second, perform the object updates
//                        enemycomponent.prevObject = enemycomponent.nearestObj;
//                        enemycomponent.nearestObj = -1;
//
//                        // Reset the timer after execution
//                        enemycomponent.timeSinceTargetReached = 0.0f;
//                    }
//                }
//            }
//            else
//            {
//                // If the target position is reached or very close, set velocity to zero
//                enemyphysic.velocity.x = 0.0f;
//                enemyphysic.velocity.y = 0.0f;
//                enemyphysic.force.x = enemyphysic.force.y = 0.f;
//
//                enemycomponent.prevObject = enemycomponent.nearestObj;
//                enemycomponent.nearestObj = -1;
//            }
//            break;
//
//        case Enemy::CHASE:
//            if (object->GetComponent<Animation>().currentClip != "Walk")
//            {
//                object->GetComponent<Animation>().SetAnimation("Walk");
//            }
//            // Compute the direction vector to the target (player)
//            enemycomponent.dirX = playerObj->GetComponent<Transform>().position.x - enemytransform.position.x;
//            enemycomponent.dirY = playerObj->GetComponent<Transform>().position.y - enemytransform.position.y;
//
//            // Calculate the magnitude of the direction vector (distance to player)
//            enemycomponent.magnitude = std::sqrt(enemycomponent.dirX * enemycomponent.dirX + enemycomponent.dirY * enemycomponent.dirY);
//
//            if (enemycomponent.magnitude > 0.0f) // Avoid division by zero
//            {
//                // Normalize the direction vector
//                enemycomponent.dirX /= enemycomponent.magnitude;
//                enemycomponent.dirY /= enemycomponent.magnitude;
//
//                // Scale the normalized direction by the enemy's speed to compute velocity
//                enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
//                enemyphysic.force.y = enemycomponent.dirY * enemycomponent.speed;
//
//                // Check if the player is within attack range
//                if (enemycomponent.type == enemycomponent.FISH)
//                {
//                    if (std::abs(playerObj->GetComponent<Transform>().position.x - enemytransform.position.x) < 150.f)
//                    {
//                        if (enemycomponent.IsPlayerInRange(playerObj->GetComponent<Transform>(), enemytransform))
//                        {
//
//                            // Stop movement and transition to ATTACK state
//                            //enemyphysic.velocity.x = 0.0f;
//                            //enemyphysic.velocity.y = 0.0f;
//                            //enemyphysic.force.x = enemyphysic.force.y = 0.f;
//
//                            enemyphysic.velocity.x = 0.0f;
//                            enemyphysic.velocity.y = 0.0f;
//                            enemyphysic.force.x = enemyphysic.force.y = 0.f;
//                            enemycomponent.state = enemycomponent.STANDBY;
//                        }
//                    }
//                }
//                if (enemycomponent.type == enemycomponent.WORM)
//                {
//                    if (std::abs(playerObj->GetComponent<Transform>().position.x - enemytransform.position.x) < 800.f)
//                    {
//                        if (enemycomponent.IsPlayerInRange(playerObj->GetComponent<Transform>(), enemytransform))
//                        {
//                            enemyphysic.velocity.x = 0.0f;
//                            enemyphysic.velocity.y = 0.0f;
//                            enemyphysic.force.x = enemyphysic.force.y = 0.f;
//                            enemycomponent.state = enemycomponent.STANDBY;
//                        }
//                    }
//                }
//                break;
//            }
//            else
//            {
//                // If the magnitude is zero (enemy is on top of the player), go to ATTACK
//                enemyphysic.velocity.x = 0.0f;
//                enemyphysic.velocity.y = 0.0f;
//                enemyphysic.force.x = enemyphysic.force.y = 0.f;
//                enemycomponent.state = enemycomponent.STANDBY;
//                break;
//            }
//
//            // Ensure other transitions (like to ROAM) don�t block the ATTACK state
//            if (IsEnemyAwayFromObject(object, playerObj, 350.0f))
//            {
//                //std::cout << "Transitioning to ROAM state for enemy: " << object->GetInstanceID() << std::endl;
//                enemycomponent.state = enemycomponent.ROAM;
//                break;
//            }
//            break;
//
//        case Enemy::STANDBY:
//            if (object->GetComponent<Animation>().currentClip != "Idle")
//            {
//                object->GetComponent<Animation>().SetAnimation("Idle");
//            }
//            if (numEnemyTarget < 2)
//            {
//                if (!enemycomponent.isWithPlayer)
//                {
//                    numEnemyTarget++;
//                    enemycomponent.state = enemycomponent.ATTACK;
//                    enemycomponent.isWithPlayer = true;
//                }
//
//            }
//
//            // Ensure other transitions (like to ROAM) don�t block the ATTACK state
//            if (IsEnemyAwayFromObject(object, playerObj, 350.0f))
//            {
//                //std::cout << "Transitioning to ROAM state for enemy: " << object->GetInstanceID() << std::endl;
//                if (enemycomponent.isWithPlayer)
//                {
//                    numEnemyTarget--;
//                    enemycomponent.state = enemycomponent.ROAM;
//                    enemycomponent.isWithPlayer = false;
//                }
//                break;
//            }
//            break;
//
//        case Enemy::ATTACK:
//
//            enemycomponent.atktimer -= static_cast<float>(g_FrameRateController.GetDeltaTime());
//
//            if (enemycomponent.atktimer < 0)
//            {
//                if (object->GetComponent<Animation>().currentClip != "Attack")
//                {
//                    object->GetComponent<Animation>().SetAnimation("Attack");
//                }
//            }
//            else
//            {
//                if (object->GetComponent<Animation>().currentClip != "Idle")
//                {
//                    object->GetComponent<Animation>().SetAnimation("Idle");
//                }
//            }
//
//            //Charge attack for fish
//            if (enemycomponent.type == enemycomponent.FISH)
//            {
//
//                if (object->GetComponent<Animation>().currentClip == "Attack" && object->GetComponent<Animation>().current_frame == 18)
//                {
//                    //SFX
//                    auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
//                    if (audioM.GetSFXindex("FishAttack") != -1)
//                    {
//                        if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("FishAttack")))
//                        {
//                            audioM.PlaySFX(audioM.GetSFXindex("FishAttack"));
//
//                            //check attack
//                            if (enemycomponent.IsPlayerInRange(playerObj->GetComponent<Transform>(), enemytransform))
//                            {
//                                //attack
//                                if (playerObj != nullptr)
//                                {
//                                    enemycomponent.AttackPlayer(playerObj->GetComponent<Player>().currentHealth);
//                                    ECS::GetInstance().GetSystem<PlayerManager>()->OnCollisionEnter(playerObj->GetInstanceID());
//                                }
//                            }
//                        }
//
//                    }
//
//                }
//
//                if (object->GetComponent<Animation>().currentClip == "Attack" && object->GetComponent<Animation>().current_frame == 22)
//                {
//                    if (!enemycomponent.IsPlayerInRange(playerObj->GetComponent<Transform>(), enemytransform))
//                    {
//                        enemycomponent.state = enemycomponent.STANDBY;
//                        enemycomponent.atktimer = 0.f;
//                        break;
//                    }
//                    else
//                    {
//                        enemycomponent.atktimer = 1.5f;
//                    }
//
//                }
//            }
//            else if (enemycomponent.type == enemycomponent.WORM)
//            {
//                if (object->GetComponent<Animation>().currentClip == "Attack" && object->GetComponent<Animation>().current_frame == 19)
//                {
//                    if (!enemycomponent.IsPlayerInRange(playerObj->GetComponent<Transform>(), enemytransform))
//                    {
//                        enemycomponent.state = enemycomponent.STANDBY;
//                        enemycomponent.atktimer = 0.f;
//                        enemycomponent.wormshoot = false;
//                        break;
//                    }
//                    else
//                    {
//                        enemycomponent.atktimer = 1.5f;
//                        enemycomponent.wormshoot = false;
//                    }
//
//                }
//
//                if (object->GetComponent<Animation>().currentClip == "Attack" && object->GetComponent<Animation>().current_frame == 15 && !enemycomponent.wormshoot)
//                {
//                    //Play SFX
//                    auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
//                    if (audioM.GetSFXindex("WormAttack") != -1)
//                    {
//                        if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("WormAttack")))
//                        {
//                            audioM.PlaySFX(audioM.GetSFXindex("WormAttack"));
//                        }
//
//                    }
//
//                    //spawn bullet
//                    static int number = 0;
//                    GameObject* cloneObject = GameObjectManager::GetInstance().GetGOByTag("EnemyProjectile1");
//
//                    if (cloneObject != nullptr)
//                    {
//                        std::string name = "bullet" + std::to_string(number++);
//                        GameObject& newObject = GameObjectManager::GetInstance().CloneObject(*cloneObject, name, "EnemyProjectile");
//
//                        newObject.GetComponent<Transform>().position.x = enemytransform.position.x;
//                        newObject.GetComponent<Transform>().position.y = enemytransform.position.y + 50.f;
//
//                        newObject.GetComponent<Animation>().SetAnimation("Projectile");
//
//                        auto playerpos = playerObj->GetComponent<Transform>().position;
//
//                        Vec2 dir;
//                        Vec2Normalize(dir, Vec2(playerpos.x - newObject.GetComponent<Transform>().position.x,
//                            playerpos.y - newObject.GetComponent<Transform>().position.y));
//
//                        newObject.GetComponent<Rigidbody2D>().velocity = dir * 500;
//
//                        float angleRad = atan2(dir.y, dir.x);
//
//                        // Convert to degrees
//                        float angleDeg = angleRad * (180.0f / 3.14159265358979323846f);
//
//                        if (newObject.GetComponent<Rigidbody2D>().velocity.x > 0)
//                        {
//                            newObject.GetComponent<SpriteRender>().flipX = true;
//                            // Apply rotation to bullet
//                            newObject.GetComponent<Transform>().rotation = angleDeg;
//
//                        }
//                        else if (newObject.GetComponent<Rigidbody2D>().velocity.x > 0)
//                        {
//                            newObject.GetComponent<SpriteRender>().flipX = false;
//                            // Apply rotation to bullet
//                            newObject.GetComponent<Transform>().rotation = -angleDeg;
//                        }
//
//                        newObject.AddComponent(EnemyBullet{});
//                        enemycomponent.wormshoot = true;
//                    }
//
//                }
//            }
//            break;
//
//        default:
//            break;
//        }
//
//        ++it; // Move to the next enemy
//    }
//}