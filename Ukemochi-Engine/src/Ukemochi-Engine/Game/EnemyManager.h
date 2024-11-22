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

        int FindNearestObject(GameObject* enemy) const;

        void UpdateEnemies();

        void ClearEnemies();
    private:
        std::vector<EntityID> enemyObjects;
        std::vector<EntityID> environmentObjects;
        int nearestID = -1;
    };

}
#endif
