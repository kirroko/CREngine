#pragma once
#ifndef ENEMYMANAGER_H
#define ENEMYMANAGER_H

#include "../ECS/ECS.h"
#include "../Factory/GameObject.h"

namespace Ukemochi
{
    class EnemyManager : public System
    {
    public:
        void UpdateEnemyList();

        void UpdateEnemies();

        void EnemyCollisionResponse(EntityID enemyID, EntityID objID);

        bool IsClearPathToPosition(GameObject* enemy, float newX, float newY);

        int FindNearestObject(GameObject* enemy) const;

        bool IsEnemyAwayFromObject(GameObject* enemy, GameObject* targetObject, float minDistanceThreshold) const;

        void ClearEnemies();
    private:
        GameObject* playerObj;
        std::vector<EntityID> enemyObjects;
        std::vector<EntityID> environmentObjects;
        int nearestID = -1;
    };

}
#endif
