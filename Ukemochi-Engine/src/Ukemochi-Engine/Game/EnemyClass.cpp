#include "PreCompile.h"
//#include 
//#include "../Math/Transformation.h"
//#include "../Factory/GameObjectManager.h"
//#include "../FrameController.h"
//#include "../Collision/Collision.h"
//
//namespace Ukemochi
//{
//    // Constructor
//    Enemy::Enemy(float startX, float startY, EnemyTypes type, EntityID ID)
//        : ID(ID), state(EnemyStates::ROAM), type(type), posX(startX), posY(startY), targetX(startX), targetY(startY), prevObject(-1), isCollide(false)
//    {
//        switch (type)
//        {
//        case Ukemochi::FISH:
//            health = 120.f;
//            attackPower = 20.f;
//            attackRange = 0.5f;
//            speed = 150.f;
//            break;
//        case Ukemochi::WORM:
//            health = 100.f;
//            attackPower = 10.f;
//            attackRange = 5.f;
//            speed = 150.f;
//            break;
//        case Ukemochi::DEFAULT:
//            break;
//        default:
//            break;
//        }
//
//        UpdateEnviromentList();
//    }
//
//    void Enemy::UpdateEnviromentList()
//    {
//        for (GameObject* object : GameObjectManager::GetInstance().GetAllGOs())
//        {
//            if (object->GetTag() == "Environment")
//            {
//                environmentObjects.push_back(object->GetInstanceID());
//            }
//        }
//    }
//
//    void Enemy::RoamState()
//    {
//        static GameObject* nearestObj = nullptr;
//        if (nearestObj == nullptr)
//        {
//            nearestObj = GameObjectManager::GetInstance().GetGO(FindNearestObject(environmentObjects));
//        }
//        else
//        {
//            float targetX = nearestObj->GetComponent<Transform>().position.x;
//            float targetY = nearestObj->GetComponent<Transform>().position.y;
//
//            // Move to the nearest object
//            MoveToTarget(targetX, targetY, g_FrameRateController.GetDeltaTime(), speed);
//
//            //when target reach find next obj
//            if (ReachedTarget(GetPosition().first, GetPosition().second, targetX, targetY) == true)
//            {
//                nearestObj = nullptr;
//            }
//
//            if (GameObjectManager::GetInstance().GetGOByTag("Player") == nullptr)
//            {
//                return;
//            }
//
//            if (ReachedTarget(GetPosition().first, GetPosition().second,
//                GameObjectManager::GetInstance().GetGOByTag("Player")->GetComponent<Transform>().position.x,
//                GameObjectManager::GetInstance().GetGOByTag("Player")->GetComponent<Transform>().position.y, 250.f) == true)
//            {
//                state = CHASE;
//                return;
//            }
//        }
//
//    }
//
//    void Enemy::ChaseState()
//    {
//        SetTarget(GameObjectManager::GetInstance().GetGOByTag("Player")->GetInstanceID());
//
//        MoveToTarget(GameObjectManager::GetInstance().GetGOByTag("Player")->GetComponent<Transform>().position.x,
//            GameObjectManager::GetInstance().GetGOByTag("Player")->GetComponent<Transform>().position.y, g_FrameRateController.GetDeltaTime(), speed);
//
//        if (ReachedTarget(GetPosition().first, GetPosition().second,
//            GameObjectManager::GetInstance().GetGOByTag("Player")->GetComponent<Transform>().position.x,
//            GameObjectManager::GetInstance().GetGOByTag("Player")->GetComponent<Transform>().position.y, 250.f) == false)
//        {
//            state = ROAM;
//            return;
//        }
//
//        //in attack range
//        if (IsPlayerInRange(GameObjectManager::GetInstance().GetGOByTag("Player")->GetInstanceID()))
//        {
//            state = ATTACK;
//            return;
//        }
//    }
//
//    void Enemy::AttackState()
//    {
//        static float timer = 0.0f;
//
//        timer -= g_FrameRateController.GetDeltaTime();
//
//        if (timer <= 0.0f)
//        {
//           //AttackPlayer(GameObjectManager::GetInstance().GetGOByTag("Player")->GetComponent<Collision>());
//           timer = 3.0f;
//        }
//    }
//
//    void Enemy::DeadState()
//    {
//        GameObjectManager::GetInstance().DestroyObject(ID);
//    }
//
//    void Enemy::SetTarget(EntityID targetID)
//    {
//        GameObject* target = GameObjectManager::GetInstance().GetGO(targetID);
//
//        this->targetX = target->GetComponent<Transform>().position.x;
//        this->targetY = target->GetComponent<Transform>().position.y;
//    }
//
//    // Set the target destination
//    void Enemy::SetTarget(float targetX, float targetY) {
//        this->targetX = targetX;
//        this->targetY = targetY;
//    }
//
//    int Enemy::FindNearestObject(const std::vector<EntityID>& environmentObjects) const {
//        if (environmentObjects.empty()) {
//            return -1; // No objects available
//        }
//
//        float minDistance = std::numeric_limits<float>::max();
//        GameObject* nearestObject = nullptr;
//
//        for (EntityID objectID : environmentObjects) {
//            GameObject* object = GameObjectManager::GetInstance().GetGO(objectID);
//            float dx = posX - object->GetComponent<Transform>().position.x;
//            float dy = posY - object->GetComponent<Transform>().position.y;
//            float distance = std::sqrt(dx * dx + dy * dy);
//
//            if (prevObject != object->GetInstanceID() && distance < minDistance) {
//                minDistance = distance;
//                nearestObject = object;
//            }
//        }
//
//        prevObject = nearestObject->GetInstanceID();
//
//        return nearestObject->GetInstanceID();
//    }
//
//    void Enemy::MoveToTarget(float targetX, float targetY, float deltaTime, float speed)
//    {
//        // Check if already at the target
//        if (ReachedTarget(posX, posY, targetX, targetY, 0.1f)) 
//        {
//            std::cout << "Enemy already at the target position.\n";
//            return;
//        }
//
//        float dx = targetX - posX;
//        float dy = targetY - posY;
//        float distance = std::sqrt(dx * dx + dy * dy);
//
//        // Normalize direction
//        if (distance > 0.0f) {
//            dx /= distance;
//            dy /= distance;
//        }
//
//        // Move the enemy
//        posX += dx * speed * deltaTime;
//        posY += dy * speed * deltaTime;
//
//        // Print position for debugging
//        std::cout << "Enemy Position: (" << posX << ", " << posY << ")" << std::endl;
//    }
//
//
//    // Check if the enemy can attack the player
//    bool Enemy::IsPlayerInRange(EntityID playerID) const {
//        GameObject* player = GameObjectManager::GetInstance().GetGO(playerID);
//        float playerX = player->GetComponent<Transform>().position.x;
//        float playerY = player->GetComponent<Transform>().position.y;
//        float dx = playerX - posX;
//        float dy = playerY - posY;
//        float distance = std::sqrt(dx * dx + dy * dy);
//        return distance <= attackRange;
//    }
//
//    // Attack the player (reduces player's health)
//    void Enemy::AttackPlayer(float& playerHealth) {
//        if (playerHealth > 0.0f) {
//            playerHealth -= attackPower;
//            if (playerHealth < 0.0f) {
//                playerHealth = 0.0f; // Ensure health does not go negative
//            }
//        }
//    }
//
//    // Get the current position of the enemy
//    std::pair<float, float> Enemy::GetPosition() const {
//        return { posX, posY };
//    }
//
//    // Get the current health of the enemy
//    float Enemy::GetHealth() const {
//        return health;
//    }
//
//    // Reduce the enemy's health when taking damage
//    void Enemy::TakeDamage(float damage) {
//        health -= damage;
//        if (health < 0.0f) {
//            health = 0.0f; // Ensure health does not go negative
//        }
//    }
//
//    EnemyStates Enemy::GetEnemyState() const
//    {
//        return state;
//    }
//
//    void Enemy::SetEnemyState(EnemyStates newState)
//    {
//        state = newState;
//    }
//
//    bool Enemy::IsCollide() const
//    {
//        return isCollide;
//    }
//
//    void Enemy::SetIsCollide(bool b)
//    {
//        isCollide = b;
//    }
//
//    // Check if two points are within a threshold distance
//    bool Enemy::ReachedTarget(float x1, float y1, float x2, float y2, float threshold) const {
//        float dx = x1 - x2;
//        float dy = y1 - y2;
//        return (dx * dx + dy * dy) <= (threshold * threshold);
//    }
//}
