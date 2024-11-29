#include "PreCompile.h"
#include "EnemyManager.h"
#include "../Factory/GameObjectManager.h"
#include "../FrameController.h"
#include "DungeonManager.h"
#include "Ukemochi-Engine/Game/PlayerManager.h" // for player data

namespace Ukemochi
{
	void EnemyManager::UpdateEnemyList()
	{
        std::srand(static_cast<unsigned int>(std::time(nullptr))); // Seed the random generator
        enemyObjects.clear();
        environmentObjects.clear();

		for (EntityID objectID : ECS::GetInstance().GetSystem<DungeonManager>()->rooms[ECS::GetInstance().GetSystem<DungeonManager>()->current_room_id].entities)
		{
            GameObject* object = GameObjectManager::GetInstance().GetGO(objectID);
			if (object->HasComponent<Enemy>())
			{
				enemyObjects.push_back(object->GetInstanceID());
			}
		}
        for (EntityID objectID : ECS::GetInstance().GetSystem<DungeonManager>()->rooms[ECS::GetInstance().GetSystem<DungeonManager>()->current_room_id].entities)
        {
            GameObject* object = GameObjectManager::GetInstance().GetGO(objectID);
            if (object->GetTag() == "WayPoint")
            {
                environmentObjects.push_back(object->GetInstanceID());
            }
        }

        playerObj = GameObjectManager::GetInstance().GetGOByTag("Player");
	}

    void EnemyManager::UpdateEnemies()
    {
        // Iterate through the enemyObjects list
        for (auto it = enemyObjects.begin(); it != enemyObjects.end();)
        {
            GameObject* object = GameObjectManager::GetInstance().GetGO(*it);
            object->GetComponent<Animation>().SetAnimation("Idle");

            if (object ->GetActive() == false)
            {
                it++;
                continue;
            }

            auto& enemycomponent = object->GetComponent<Enemy>();
            auto& enemyphysic = object->GetComponent<Rigidbody2D>();
            auto& enemytransform = object->GetComponent<Transform>();

            if (enemycomponent.health <= 0.f)
            {
                enemycomponent.state = Enemy::DEAD;
            }
            else
            {
                object->SetActive(true);
            }

            // If the enemy is in DEAD state, remove it from the list after processing DeadState
            if (enemycomponent.state == Enemy::DEAD)
            {
                auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                //dont overlap kick sound
                if ((!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("Pattack3")))&& !enemycomponent.isDead)
                {
                    audioM.PlaySFX(audioM.GetSFXindex("EnemyKilled"));
                }
                object->SetActive(false);
                enemycomponent.isDead = true;
                it++;
                //GameObjectManager::GetInstance().DestroyObject(object->GetInstanceID());
                //it = enemyObjects.erase(it); // Remove the enemy from the list
                continue; // Skip further processing for this enemy
            }

            //if there no nearest obj, find neareast obj
            if (enemycomponent.nearestObj == -1)
            {
                enemycomponent.nearestObj = FindNearestObject(object);
            }

            if (enemycomponent.state != enemycomponent.ATTACK)
            {
                auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                if (enemycomponent.type == enemycomponent.FISH)
                {
                    if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("FishMove")))
                    {
                        audioM.PlaySFX(audioM.GetSFXindex("FishMove"));
                    }
                }
            }

            if (playerObj != nullptr)
            {
                if (enemycomponent.ReachedTarget(enemytransform.position.x, enemytransform.position.y,
                    playerObj->GetComponent<Transform>().position.x,
                    playerObj->GetComponent<Transform>().position.y, 350.f) == true && enemycomponent.state == enemycomponent.ROAM)
                {
                    //std::cout << object->GetInstanceID() << " Chase" << std::endl;
                    enemycomponent.state = enemycomponent.CHASE;
                }
            }

            // Skip collision handling for dead enemies
            if (enemycomponent.isCollide)
            {

                if (IsEnemyAwayFromObject(object, GameObjectManager::GetInstance().GetGO(enemycomponent.nearestObj),300.f) && enemycomponent.state == enemycomponent.ROAM)
                {
                    enemycomponent.nearestObj = -1;
                    enemycomponent.isCollide = false;
                }


                //enemycomponent.WrapToTarget(object->GetComponent<Transform>(), enemycomponent.targetX, enemycomponent.targetY, g_FrameRateController.GetDeltaTime(), enemycomponent.speed);
                //
                //if (enemycomponent.ReachedTarget(
                //    enemycomponent.posX,
                //    enemycomponent.posY,
                //    enemycomponent.targetX,
                //    enemycomponent.targetY, 0.2f))
                //{
                //    enemycomponent.isCollide = false;
                //}

                ++it;
                continue;
            }


            // Handle other states
            switch (enemycomponent.state)
            {
            case Enemy::ROAM:

                // Compute the direction vector to the target waypoint
                enemycomponent.dirX = enemycomponent.targetX - enemytransform.position.x;
                enemycomponent.dirY = enemycomponent.targetY - enemytransform.position.y;

                // Calculate the distance to normalize the direction
                enemycomponent.magnitude = std::sqrt(enemycomponent.dirX * enemycomponent.dirX + enemycomponent.dirY * enemycomponent.dirY);

                if (enemycomponent.magnitude > 0.0f) // Avoid division by zero
                {
                    // Normalize the direction vector
                    enemycomponent.dirX /= enemycomponent.magnitude;
                    enemycomponent.dirY /= enemycomponent.magnitude;

                    // Scale the normalized direction by the enemy's speed
                    enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                    enemyphysic.force.y = enemycomponent.dirY * enemycomponent.speed;

                    if (enemycomponent.ReachedTarget(enemyphysic.position.x, enemyphysic.position.y, enemycomponent.targetX, enemycomponent.targetY,20.f))
                    {
                        enemyphysic.velocity.x = 0.0f;
                        enemyphysic.velocity.y = 0.0f;
                        enemyphysic.force.x = enemyphysic.force.y = 0.f;
                        
                        enemycomponent.prevObject = enemycomponent.nearestObj;
                        enemycomponent.nearestObj = -1;
                    }
                }
                else
                {
                    // If the target position is reached or very close, set velocity to zero
                    enemyphysic.velocity.x = 0.0f;
                    enemyphysic.velocity.y = 0.0f;
                    enemyphysic.force.x = enemyphysic.force.y = 0.f;

                    enemycomponent.prevObject = enemycomponent.nearestObj;
                    enemycomponent.nearestObj = -1;
                }

                ////enemycomponent.SetTarget(GameObjectManager::GetInstance().GetGO(enemycomponent.nearestObj)->GetComponent<Transform>());
                //enemycomponent.MoveToTarget(object->GetComponent<Transform>(), enemycomponent.targetX, enemycomponent.targetY, g_FrameRateController.GetDeltaTime(), enemycomponent.speed);

                break;

            case Enemy::CHASE:
                
                //std::cout << object->GetInstanceID() << " Chase" << std::endl;

                // Compute the direction vector to the target (player)
                enemycomponent.dirX = playerObj->GetComponent<Transform>().position.x - enemytransform.position.x;
                enemycomponent.dirY = playerObj->GetComponent<Transform>().position.y - enemytransform.position.y;

                // Calculate the magnitude of the direction vector (distance to player)
                enemycomponent.magnitude = std::sqrt(enemycomponent.dirX * enemycomponent.dirX + enemycomponent.dirY * enemycomponent.dirY);

                if (enemycomponent.magnitude > 0.0f) // Avoid division by zero
                {
                    // Normalize the direction vector
                    enemycomponent.dirX /= enemycomponent.magnitude;
                    enemycomponent.dirY /= enemycomponent.magnitude;

                    // Scale the normalized direction by the enemy's speed to compute velocity
                    enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                    enemyphysic.force.y = enemycomponent.dirY * enemycomponent.speed;

                    // Check if the player is within attack range
                    if (enemycomponent.IsPlayerInRange(playerObj->GetComponent<Transform>(),enemytransform))
                    {
                        // Stop movement and transition to ATTACK state
                        enemyphysic.velocity.x = 0.0f;
                        enemyphysic.velocity.y = 0.0f;
                        enemyphysic.force.x = enemyphysic.force.y = 0.f;
                        enemycomponent.state = enemycomponent.ATTACK;
                        break;
                    }
                }
                else
                {
                    // If the magnitude is zero (enemy is on top of the player), go to ATTACK
                    enemyphysic.velocity.x = 0.0f;
                    enemyphysic.velocity.y = 0.0f;
                    enemyphysic.force.x = enemyphysic.force.y = 0.f;
                    enemycomponent.state = enemycomponent.ATTACK;
                    break;
                }

                // Ensure other transitions (like to ROAM) don�t block the ATTACK state
                if (IsEnemyAwayFromObject(object, playerObj, 350.0f))
                {
                    //std::cout << "Transitioning to ROAM state for enemy: " << object->GetInstanceID() << std::endl;
                    enemycomponent.state = enemycomponent.ROAM;
                    break;
                }

                //enemycomponent.MoveToTarget(object->GetComponent<Transform>(), playerObj->GetComponent<Transform>().position.x,
                //    playerObj->GetComponent<Transform>().position.y, g_FrameRateController.GetDeltaTime(), enemycomponent.speed);

                //if (enemycomponent.ReachedTarget(enemycomponent.posX, enemycomponent.posY,
                //    playerObj->GetComponent<Transform>().position.x,
                //    playerObj->GetComponent<Transform>().position.y, 250.f) == false)
                //{
                //    enemycomponent.state = enemycomponent.ROAM;
                //    return;
                //}
                break;

            case Enemy::ATTACK:

                //std::cout << object->GetInstanceID() << " Attack" << std::endl;

                if (!enemycomponent.IsPlayerInRange(playerObj->GetComponent<Transform>(), enemytransform))
                {
                    enemycomponent.state = enemycomponent.CHASE;
                    break;
                }
                //if (enemycomponent.IsPlayerInRange(playerObj->GetComponent<Transform>()) == false)
                //{
                //    enemycomponent.state = enemycomponent.CHASE;
                //    break;
                //}

                enemycomponent.atktimer -= static_cast<float>(g_FrameRateController.GetDeltaTime());

                if (enemycomponent.atktimer <= 1.0f)
                {
                    //Charge attack for fish
                    //shoot for worm
                    static bool attack = false;
                    auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                    if (enemycomponent.type == enemycomponent.FISH)
                    {
                        if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("FishAttack")) && !attack)
                        {
                            audioM.PlaySFX(audioM.GetSFXindex("FishAttack"));
                            attack = true;
                        }
                        else
                        {
                            //temp
                            if (playerObj != nullptr)
                            {
                                enemycomponent.AttackPlayer(playerObj->GetComponent<Player>().maxHealth);
                                ECS::GetInstance().GetSystem<PlayerManager>()->OnCollisionEnter(playerObj->GetInstanceID());
                            }

                            std::cout << "player hit\n";
                            enemycomponent.atktimer = 5.f;
                            attack = false;
                        }
                    }

                }
                break;

            default:
                break;
            }

            ++it; // Move to the next enemy
        }
    }

    void EnemyManager::EnemyCollisionResponse(EntityID enemyID, EntityID objID)
    {
        auto& gameObjectManager = GameObjectManager::GetInstance();
        auto enemy = gameObjectManager.GetGO(enemyID);
        auto obj2 = gameObjectManager.GetGO(objID);

        auto& enemyComponent = enemy->GetComponent<Enemy>();
        auto& enemyTransform = enemy->GetComponent<Transform>();
        auto& obj2Transform = obj2->GetComponent<Transform>();

        if (obj2->HasComponent<Enemy>())
        {
            auto& enemyComponent2 = obj2->GetComponent<Enemy>();
            enemyComponent2.isCollide = false;
            enemyComponent.isCollide = false;
        }
        else
        {
            //set collide to true then now the obj is the obj save the pathfinding obj as prev
            enemyComponent.isCollide = true;
            enemyComponent.prevObject = static_cast<int>(enemyComponent.nearestObj);
            enemyComponent.nearestObj = static_cast<int>(objID);
        }


        

        /*
        //auto& gameObjectManager = GameObjectManager::GetInstance();
        //auto enemy = gameObjectManager.GetGO(enemyID);
        //auto environment = gameObjectManager.GetGO(objID);

        //auto& enemyComponent = enemy->GetComponent<Enemy>();
        //auto& enemyTransform = enemy->GetComponent<Transform>();
        //auto& envTransform = environment->GetComponent<Transform>();

        //// Mark the enemy's state
        //if (enemyComponent.nearestObj == objID)
        //{
        //    enemyComponent.prevObject = objID;
        //    enemyComponent.nearestObj = FindNearestObject(enemy);
        //}
        //if (enemyComponent.nearestObj == -1)
        //{
        //    return;
        //}
        //enemyComponent.isCollide = true;

        //// Determine if the environment object is on the right or left side of the enemy
        //bool isRightSide = envTransform.position.x > enemyTransform.position.x;

        //// Define potential new positions around the environment object
        //std::vector<std::pair<float, float>> potentialPositions;

        //if (isRightSide)
        //{
        //    //env obj is on right
        //    std::cout << "R ENEMY < ENV" << std::endl;
        //    //if nearest is on ride or left

        //    //if nearest obj on right go top right or bottom right
        //    if (gameObjectManager.GetGO(enemyComponent.nearestObj)->GetComponent<Transform>().position.x > enemyTransform.position.x)// right
        //    {
        //        std::cout << "R ENEMY < nearest" << std::endl;
        //        // Environment is on the right side
        //        potentialPositions = {
        //            {envTransform.position.x + 0.6f * envTransform.scale.x, envTransform.position.y + 1.5f * envTransform.scale.y}, // Top right
        //            {envTransform.position.x + 0.6f * envTransform.scale.x, envTransform.position.y - 1.5f * envTransform.scale.y}  // Bottom right
        //        };
        //    }
        //    else
        //    {
        //        std::cout << "L ENV < nearest" << std::endl;
        //        // Environment is on the left side
        //        potentialPositions = {
        //            {envTransform.position.x - 0.6f * envTransform.scale.x, envTransform.position.y + 1.5f * envTransform.scale.y}, // Top left
        //            {envTransform.position.x - 0.6f * envTransform.scale.x, envTransform.position.y - 1.5f * envTransform.scale.y}  // Bottom left
        //        };
        //    }


        //}
        //else
        //{
        //    std::cout << "L ENV < ENEMY" << std::endl;
        //    // Environment is on the left side

        //    if (gameObjectManager.GetGO(enemyComponent.nearestObj)->GetComponent<Transform>().position.x > enemyTransform.position.x)
        //    {
        //        std::cout << "R ENEMY < nearest" << std::endl;
        //        // Environment is on the right side
        //        potentialPositions = {
        //            {envTransform.position.x + 0.6f * envTransform.scale.x, envTransform.position.y + 1.5f * envTransform.scale.y}, // Top right
        //            {envTransform.position.x + 0.6f * envTransform.scale.x, envTransform.position.y - 1.5f * envTransform.scale.y}  // Bottom right
        //        };
        //    }
        //    else
        //    {
        //        std::cout << "L ENV < nearest" << std::endl;
        //        // Environment is on the left side
        //        potentialPositions = {
        //            {envTransform.position.x - 0.6f * envTransform.scale.x, envTransform.position.y + 1.5f * envTransform.scale.y}, // Top left
        //            {envTransform.position.x - 0.6f * envTransform.scale.x, envTransform.position.y - 1.5f * envTransform.scale.y}  // Bottom left
        //        };
        //    }


        //}

        //// Find the closest position to the enemy
        //std::pair<float, float> closestPosition;
        //float minDistance = std::numeric_limits<float>::max();

        //for (const auto& pos : potentialPositions)
        //{
        //    // Calculate the distance to the potential position
        //    float dx = pos.first - enemyTransform.position.x;
        //    float dy = pos.second - enemyTransform.position.y;
        //    float distance = std::sqrt(dx * dx + dy * dy);

        //    // Check if this is the closest position and the path is clear
        //    if (distance < minDistance && IsClearPathToPosition(enemy, pos.first, pos.second))
        //    {
        //        minDistance = distance;
        //        closestPosition = pos;
        //    }
        //}

        //// If a valid closest position is found, set it as the target
        //if (minDistance < std::numeric_limits<float>::max())
        //{
        //    enemyComponent.targetX = closestPosition.first;
        //    enemyComponent.targetY = closestPosition.second;
        //}
        */

    }


    bool EnemyManager::IsClearPathToPosition(GameObject* enemy, float newX, float newY)
    {
        // Check if moving the enemy to the new position (newX, newY) is clear of obstacles

        for (EntityID objectID : environmentObjects)
        {
            GameObject* object = GameObjectManager::GetInstance().GetGO(objectID);
            Transform& objectTransform = object->GetComponent<Transform>();

            // Calculate if the potential new position collides with any other environment object
            float dx = newX - objectTransform.position.x;
            float dy = newY - objectTransform.position.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            if (newX < enemy->GetComponent<Transform>().scale.x || newX > 1600 || newY < 0 || newY > 900)
            {
                return false;
            }

            // If the new position is too close to the environment object, it's not clear
            if (distance < 1.0f)  // Adjust this threshold as needed
            {
                return false;
            }
        }

        return true;  // The path is clear to move
    }


    int EnemyManager::FindNearestObject(GameObject* enemy) const
    {
        if (environmentObjects.empty()) {
            return -1; // No objects available
        }

        // Collect all eligible objects excluding the previous object
        std::vector<GameObject*> eligibleObjects;
        for (EntityID objectID : environmentObjects) {
            GameObject* object = GameObjectManager::GetInstance().GetGO(objectID);
            if (object->GetInstanceID() != enemy->GetComponent<Enemy>().prevObject) {
                eligibleObjects.push_back(object);
            }
        }

        // If there are no eligible objects, return -1
        if (eligibleObjects.empty()) {
            return -1;
        }

        // Select a random object from the eligible list
        int randomIndex = std::rand() % eligibleObjects.size();
        GameObject* selectedObject = eligibleObjects[randomIndex];

        // Set the target position for the enemy
        enemy->GetComponent<Enemy>().targetX = selectedObject->GetComponent<Transform>().position.x;
        enemy->GetComponent<Enemy>().targetY = selectedObject->GetComponent<Transform>().position.y;

        return static_cast<int>(selectedObject->GetInstanceID());
    }

    bool EnemyManager::IsEnemyAwayFromObject(GameObject* enemy, GameObject* targetObject, float minDistanceThreshold) const
    {
        if (!enemy || !targetObject) {
            return true; // Consider "away" if either is null
        }

        // Get positions
        const auto& enemyPos = enemy->GetComponent<Transform>().position;
        const auto& objectPos = targetObject->GetComponent<Transform>().position;

        // Calculate squared distance (avoids costly sqrt operation for performance)
        float dx = enemyPos.x - objectPos.x;
        float dy = enemyPos.y - objectPos.y;
        float distanceSquared = dx * dx + dy * dy;

        // Compare with squared threshold
        return distanceSquared > (minDistanceThreshold * minDistanceThreshold);
    }

    void EnemyManager::ClearEnemies()
    {
        if (!enemyObjects.empty())
        {
            for (auto it = enemyObjects.begin(); it != enemyObjects.end();)
            {
                GameObject* object = GameObjectManager::GetInstance().GetGO(*it);
                if (object->GetComponent<Enemy>().state == Enemy::DEAD)
                {
                    GameObjectManager::GetInstance().DestroyObject(object->GetInstanceID());
                }
            }
            enemyObjects.clear();
        }
    }

}
