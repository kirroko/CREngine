#include "PreCompile.h"
#include "EnemyManager.h"
#include "../Factory/GameObjectManager.h"
#include "../FrameController.h"

namespace Ukemochi
{
	void EnemyManager::UpdateEnemyList()
	{
        enemyObjects.clear();
		for (GameObject* object : GameObjectManager::GetInstance().GetAllGOs())
		{
			if (object->HasComponent<Enemy>())
			{
				enemyObjects.push_back(object->GetInstanceID());
			}
            if (object->GetTag() == "Environment")
            {
                environmentObjects.push_back(object->GetInstanceID());
            }
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
            if (newX < enemy->GetComponent<Transform>().scale.x || newX > 1600 || newY <0 ||newY > 900)
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
            float dx = enemy->GetComponent<Enemy>().GetPosition().first - object->GetComponent<Transform>().position.x;
            float dy = enemy->GetComponent<Enemy>().GetPosition().first - object->GetComponent<Transform>().position.y;
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

    void EnemyManager::UpdateEnemies()
    {
        // Iterate through the enemyObjects list
        for (auto it = enemyObjects.begin(); it != enemyObjects.end();)
        {
            GameObject* object = GameObjectManager::GetInstance().GetGO(*it);

            //std::cout << object->GetComponent<Enemy>().prevObject << std::endl;

            if (object->GetComponent<Enemy>().GetHealth() <= 0.f)
            {
                object->GetComponent<Enemy>().state = Enemy::DEAD;
            }

            // If the enemy is in DEAD state, remove it from the list after processing DeadState
            if (object->GetComponent<Enemy>().state == Enemy::DEAD)
            {
                GameObjectManager::GetInstance().DestroyObject(object->GetInstanceID());
                it = enemyObjects.erase(it); // Remove the enemy from the list
                continue; // Skip further processing for this enemy
            }

            // Skip collision handling for dead enemies
            if (object->GetComponent<Enemy>().isCollide)
            {
                object->GetComponent<Enemy>().WrapToTarget(object->GetComponent<Transform>(), object->GetComponent<Enemy>().targetX, object->GetComponent<Enemy>().targetY, g_FrameRateController.GetDeltaTime(), object->GetComponent<Enemy>().speed);
                
                if (object->GetComponent<Enemy>().ReachedTarget(
                    object->GetComponent<Enemy>().posX,
                    object->GetComponent<Enemy>().posY,
                    object->GetComponent<Enemy>().targetX,
                    object->GetComponent<Enemy>().targetY, 0.2f))
                {
                    object->GetComponent<Enemy>().isCollide = false;
                }
                ++it;
                continue;
            }

            // Handle other states
            switch (object->GetComponent<Enemy>().state)
            {
            case Enemy::ROAM:
                if (object->GetComponent<Enemy>().nearestObj == -1)
                {
                    object->GetComponent<Enemy>().nearestObj = FindNearestObject(object);
                }
                object->GetComponent<Enemy>().RoamState(object->GetComponent<Transform>(),environmentObjects, GameObjectManager::GetInstance().GetGO(object->GetComponent<Enemy>().nearestObj)->GetComponent<Transform>()
                , GameObjectManager::GetInstance().GetGOByTag("Player")->GetInstanceID(), GameObjectManager::GetInstance().GetGOByTag("Player")->GetComponent<Transform>());
                break;
            case Enemy::CHASE:
                object->GetComponent<Enemy>().ChaseState(object->GetComponent<Transform>(),GameObjectManager::GetInstance().GetGOByTag("Player")->GetComponent<Transform>());
                break;
            case Enemy::ATTACK:
                object->GetComponent<Enemy>().AttackState();
                break;
            default:
                break;
            }

            ++it; // Move to the next enemy
        }
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
