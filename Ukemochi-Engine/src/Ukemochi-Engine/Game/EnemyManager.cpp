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
        enemyObjects.clear();
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
            if (object->GetTag() == "Environment")
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

            auto& enemycomponent = object->GetComponent<Enemy>();

            //std::cout << object->GetComponent<Enemy>().prevObject << std::endl;

            if (enemycomponent.health <= 0.f)
            {
                enemycomponent.state = Enemy::DEAD;
            }

            // If the enemy is in DEAD state, remove it from the list after processing DeadState
            if (enemycomponent.state == Enemy::DEAD)
            {
                GameObjectManager::GetInstance().DestroyObject(object->GetInstanceID());
                it = enemyObjects.erase(it); // Remove the enemy from the list
                continue; // Skip further processing for this enemy
            }

            if (playerObj != nullptr && enemycomponent.state != Enemy::ATTACK)
            {
                if (enemycomponent.ReachedTarget(enemycomponent.posX, enemycomponent.posY,
                    playerObj->GetComponent<Transform>().position.x,
                    playerObj->GetComponent<Transform>().position.y, 250.f) == true)
                {
                    enemycomponent.isCollide = false;
                    enemycomponent.state = enemycomponent.CHASE;
                }
            }


            // Skip collision handling for dead enemies
            if (enemycomponent.isCollide)
            {
                enemycomponent.WrapToTarget(object->GetComponent<Transform>(), enemycomponent.targetX, enemycomponent.targetY, g_FrameRateController.GetDeltaTime(), enemycomponent.speed);
                
                if (enemycomponent.ReachedTarget(
                    enemycomponent.posX,
                    enemycomponent.posY,
                    enemycomponent.targetX,
                    enemycomponent.targetY, 0.2f))
                {
                    enemycomponent.isCollide = false;
                }
                ++it;
                continue;
            }


            // Handle other states
            switch (enemycomponent.state)
            {
            case Enemy::ROAM:
                //if there no nearest obj, find neareast obj
                if (enemycomponent.nearestObj == -1)
                {
                    enemycomponent.nearestObj = FindNearestObject(object);
                }

                //enemycomponent.SetTarget(GameObjectManager::GetInstance().GetGO(enemycomponent.nearestObj)->GetComponent<Transform>());
                enemycomponent.MoveToTarget(object->GetComponent<Transform>(), enemycomponent.targetX, enemycomponent.targetY, g_FrameRateController.GetDeltaTime(), enemycomponent.speed);

                break;

            case Enemy::CHASE:

                enemycomponent.MoveToTarget(object->GetComponent<Transform>(), playerObj->GetComponent<Transform>().position.x,
                    playerObj->GetComponent<Transform>().position.y, g_FrameRateController.GetDeltaTime(), enemycomponent.speed);

                if (enemycomponent.ReachedTarget(enemycomponent.posX, enemycomponent.posY,
                    playerObj->GetComponent<Transform>().position.x,
                    playerObj->GetComponent<Transform>().position.y, 250.f) == false)
                {
                    enemycomponent.state = enemycomponent.ROAM;
                    return;
                }

                // in attack range
                if (enemycomponent.IsPlayerInRange(playerObj->GetComponent<Transform>()))
                {
                    enemycomponent.state = enemycomponent.ATTACK;
                    break;
                }
                break;

            case Enemy::ATTACK:

                if (enemycomponent.IsPlayerInRange(playerObj->GetComponent<Transform>()) == false)
                {
                    enemycomponent.state = enemycomponent.CHASE;
                    break;
                }
                enemycomponent.atktimer -= g_FrameRateController.GetDeltaTime();

                if (enemycomponent.atktimer <= 0.0f)
                {
                    //Charge attack for fish
                    //shoot for worm

                    //temp
                    if (playerObj != nullptr)
                    {
                        enemycomponent.AttackPlayer(playerObj->GetComponent<Player>().maxHealth);
                        ECS::GetInstance().GetSystem<PlayerManager>()->OnCollisionEnter(playerObj->GetInstanceID());
                    }

                    std::cout << "player hit\n";
                    enemycomponent.atktimer = 3.0f;
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
        auto environment = gameObjectManager.GetGO(objID);

        auto& enemyComponent = enemy->GetComponent<Enemy>();
        auto& enemyTransform = enemy->GetComponent<Transform>();
        auto& envTransform = environment->GetComponent<Transform>();

        // Mark the enemy's state
        if (enemyComponent.nearestObj == objID)
        {
            enemyComponent.prevObject = objID;
            enemyComponent.nearestObj = FindNearestObject(enemy);
        }
        if (enemyComponent.nearestObj == -1)
        {
            return;
        }
        enemyComponent.isCollide = true;

        // Determine if the environment object is on the right or left side of the enemy
        bool isRightSide = envTransform.position.x > enemyTransform.position.x;

        // Define potential new positions around the environment object
        std::vector<std::pair<float, float>> potentialPositions;

        if (isRightSide)
        {
            //env obj is on right
            std::cout << "R ENEMY < ENV" << std::endl;
            //if nearest is on ride or left

            //if nearest obj on right go top right or bottom right
            if (gameObjectManager.GetGO(enemyComponent.nearestObj)->GetComponent<Transform>().position.x > enemyTransform.position.x)// right
            {
                std::cout << "R ENEMY < nearest" << std::endl;
                // Environment is on the right side
                potentialPositions = {
                    {envTransform.position.x + 0.6f * envTransform.scale.x, envTransform.position.y + 1.5f * envTransform.scale.y}, // Top right
                    {envTransform.position.x + 0.6f * envTransform.scale.x, envTransform.position.y - 1.5f * envTransform.scale.y}  // Bottom right
                };
            }
            else
            {
                std::cout << "L ENV < nearest" << std::endl;
                // Environment is on the left side
                potentialPositions = {
                    {envTransform.position.x - 0.6f * envTransform.scale.x, envTransform.position.y + 1.5f * envTransform.scale.y}, // Top left
                    {envTransform.position.x - 0.6f * envTransform.scale.x, envTransform.position.y - 1.5f * envTransform.scale.y}  // Bottom left
                };
            }


        }
        else
        {
            std::cout << "L ENV < ENEMY" << std::endl;
            // Environment is on the left side

            if (gameObjectManager.GetGO(enemyComponent.nearestObj)->GetComponent<Transform>().position.x > enemyTransform.position.x)
            {
                std::cout << "R ENEMY < nearest" << std::endl;
                // Environment is on the right side
                potentialPositions = {
                    {envTransform.position.x + 0.6f * envTransform.scale.x, envTransform.position.y + 1.5f * envTransform.scale.y}, // Top right
                    {envTransform.position.x + 0.6f * envTransform.scale.x, envTransform.position.y - 1.5f * envTransform.scale.y}  // Bottom right
                };
            }
            else
            {
                std::cout << "L ENV < nearest" << std::endl;
                // Environment is on the left side
                potentialPositions = {
                    {envTransform.position.x - 0.6f * envTransform.scale.x, envTransform.position.y + 1.5f * envTransform.scale.y}, // Top left
                    {envTransform.position.x - 0.6f * envTransform.scale.x, envTransform.position.y - 1.5f * envTransform.scale.y}  // Bottom left
                };
            }


        }

        // Find the closest position to the enemy
        std::pair<float, float> closestPosition;
        float minDistance = std::numeric_limits<float>::max();

        for (const auto& pos : potentialPositions)
        {
            // Calculate the distance to the potential position
            float dx = pos.first - enemyTransform.position.x;
            float dy = pos.second - enemyTransform.position.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            // Check if this is the closest position and the path is clear
            if (distance < minDistance && IsClearPathToPosition(enemy, pos.first, pos.second))
            {
                minDistance = distance;
                closestPosition = pos;
            }
        }

        // If a valid closest position is found, set it as the target
        if (minDistance < std::numeric_limits<float>::max())
        {
            enemyComponent.targetX = closestPosition.first;
            enemyComponent.targetY = closestPosition.second;
        }
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

        float minDistance = std::numeric_limits<float>::max();
        GameObject* nearestObject = nullptr;

        for (EntityID objectID : environmentObjects) {
            GameObject* object = GameObjectManager::GetInstance().GetGO(objectID);
            float dx = enemy->GetComponent<Enemy>().posX - object->GetComponent<Transform>().position.x;
            float dy = enemy->GetComponent<Enemy>().posY - object->GetComponent<Transform>().position.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (enemy->GetComponent<Enemy>().prevObject != object->GetInstanceID() && distance < minDistance) {
                minDistance = distance;
                nearestObject = object;
                enemy->GetComponent<Enemy>().targetX = nearestObject->GetComponent<Transform>().position.x;
                enemy->GetComponent<Enemy>().targetY = nearestObject->GetComponent<Transform>().position.y;
            }
        }

        if (nearestObject == nullptr)
        {
            return -1;
        }

        return nearestObject->GetInstanceID();
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
