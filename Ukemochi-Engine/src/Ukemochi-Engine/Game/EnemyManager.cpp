#include "PreCompile.h"
#include "EnemyManager.h"
#include "../Factory/GameObjectManager.h"

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
            }
        }

        if (nearestObject == nullptr)
        {
            enemy->GetComponent<Enemy>().prevObject = -1;
        }
        else
        {
            enemy->GetComponent<Enemy>().prevObject = nearestObject->GetInstanceID();
        }

        return enemy->GetComponent<Enemy>().prevObject;
    }

    void EnemyManager::UpdateEnemies()
    {
        // Iterate through the enemyObjects list
        for (auto it = enemyObjects.begin(); it != enemyObjects.end();)
        {
            GameObject* object = GameObjectManager::GetInstance().GetGO(*it);

            if (object->GetComponent<Enemy>().GetHealth() <= 0.f)
            {
                object->GetComponent<Enemy>().SetEnemyState(Enemy::DEAD);
            }

            // If the enemy is in DEAD state, remove it from the list after processing DeadState
            if (object->GetComponent<Enemy>().GetEnemyState() == Enemy::DEAD)
            {
                GameObjectManager::GetInstance().DestroyObject(object->GetInstanceID());
                it = enemyObjects.erase(it); // Remove the enemy from the list
                continue; // Skip further processing for this enemy
            }

            // Skip collision handling for dead enemies
            if (object->GetComponent<Enemy>().IsCollide())
            {
                ++it;
                continue;
            }

            // Handle other states
            switch (object->GetComponent<Enemy>().GetEnemyState())
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
                if (object->GetComponent<Enemy>().GetEnemyState() == Enemy::DEAD)
                {
                    GameObjectManager::GetInstance().DestroyObject(object->GetInstanceID());
                }
            }
            enemyObjects.clear();
        }
    }

}
