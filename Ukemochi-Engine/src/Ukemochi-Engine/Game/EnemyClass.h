//#pragma once
//#ifndef ENEMY_H
//#define ENEMY_H
//
//#include <vector>
//#include <utility>
//#include <cmath>
//#include "../ECS/ECS.h"
//
//namespace Ukemochi
//{
//    enum EnemyStates
//    {
//        ROAM,
//        CHASE,
//        ATTACK,
//        DEAD,
//    };
//
//    enum EnemyTypes
//    {
//        FISH,
//        WORM,
//        DEFAULT,
//    };
//
//    class Enemy {
//    public:
//        Enemy(float startX, float startY, EnemyTypes type, EntityID ID);
//
//        void UpdateEnviromentList();
//
//        void RoamState();
//        void ChaseState();
//        void AttackState();
//        void DeadState();
//
//        void SetTarget(EntityID targetID);
//        void SetTarget(float targetX, float targetY);
//
//        int FindNearestObject(const std::vector<EntityID>& environmentObjects) const;
//        void MoveToTarget(float targetX, float targetY, float deltaTime, float speed);
//
//        void AttackPlayer(float& playerHealth);
//        bool IsPlayerInRange(EntityID player) const;
//
//        std::pair<float, float> GetPosition() const;
//        float GetHealth() const;
//        void TakeDamage(float damage);
//
//        EnemyStates GetEnemyState() const;
//        void SetEnemyState(EnemyStates newState);
//
//        bool IsCollide() const;
//        void SetIsCollide(bool b);
//
//
//    private:
//        EntityID ID;
//        EnemyStates state;
//        EnemyTypes type;
//        float posX, posY;
//        float targetX, targetY;
//        float health;
//        float attackPower;
//        float attackRange;
//        float speed;
//        std::vector<EntityID> environmentObjects;
//        mutable int prevObject;
//        bool isCollide;
//        bool ReachedTarget(float x1, float y1, float x2, float y2, float threshold = 0.1f) const;
//    };
//}
//#endif // ENEMY_H
