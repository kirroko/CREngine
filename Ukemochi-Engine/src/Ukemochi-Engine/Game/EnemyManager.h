/* Start Header ************************************************************************/
/*!
\file       EnemyManager.h
\author     Tan Si Han, t.sihan, 2301264, t.sihan@digipen.edu
\date       Sept 20, 2024
\brief      This file contains the declaration of the EnemyManager class and related methods.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

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
        /*!***********************************************************************
        \brief
            Updates the list of enemies in the game.
        \details
            This method performs any necessary updates on the enemies currently managed by the system.
        *************************************************************************/
        void UpdateEnemyList();

        /*!***********************************************************************
        \brief
            Updates the behavior of all enemies.
        \details
            This method is called to update each enemy's behavior and logic during the game's update phase.
        *************************************************************************/
        void UpdateEnemies();

        /*!***********************************************************************
        \brief
            Raytracing for enemy to see player
        \details
            This method is called to update each enemy's behavior and logic during the game's update phase.
        *************************************************************************/
        bool HasClearLineOfSight(GameObject* enemy, const Transform& player);

        /*!***********************************************************************
        \brief
            check enemy infront got object
        \details
            This method is called to update each enemy's behavior and logic during the game's update phase.
        *************************************************************************/
        bool IsObstacleInFront(float startX, float startY, float dirX, float dirY, float range);

        /*!***********************************************************************
        \brief
            Handles the collision response when an enemy collides with another object.
        \param enemyID: The ID of the enemy involved in the collision.
        \param objID: The ID of the object involved in the collision.
        \details
            This method processes the appropriate response when an enemy collides with another entity,
            which could include interactions with the player or other objects.
        *************************************************************************/
        void EnemyCollisionResponse(EntityID enemyID, EntityID objID);

        /*!***********************************************************************
        \brief
            Determines if there is a clear path for an enemy to move to a new position.
        \param enemy: Pointer to the GameObject representing the enemy.
        \param newX: The target X coordinate for the enemy to move to.
        \param newY: The target Y coordinate for the enemy to move to.
        \return
            Returns true if the enemy can move to the new position without obstruction.
        *************************************************************************/
        bool IsClearPathToPosition(GameObject *enemy, float newX, float newY);

        /*!***********************************************************************
        \brief
            Finds the nearest object to a given enemy.
        \param enemy: Pointer to the GameObject representing the enemy.
        \return
            The ID of the nearest object to the enemy.
        *************************************************************************/
        int FindNearestObject(GameObject *enemy) const;

        /*!***********************************************************************
        \brief
            Checks if an enemy is far enough away from a target object.
        \param enemy: Pointer to the GameObject representing the enemy.
        \param targetObject: Pointer to the GameObject representing the target.
        \param minDistanceThreshold: The minimum distance threshold to check.
        \return
            Returns true if the enemy is farther than the given threshold from the target object.
        *************************************************************************/
        bool IsEnemyAwayFromObject(GameObject *enemy, GameObject *targetObject, float minDistanceThreshold) const;

        /*!***********************************************************************
        \brief
            Clears all enemy entities.
        \details
            This method removes all enemies from the enemy list and performs any necessary cleanup.
        *************************************************************************/
        void ClearEnemies();

        /*!***********************************************************************
        \brief
            Flag to track when enemy is killed
        \param tracker: bool that toggles when enemmies die
        \return
            bool that act as a flag
        *************************************************************************/
        //bool KillEnemyFlag(bool& killTracker);
        int numEnemyTarget = 0;
    private:
        GameObject *playerObj;
        std::vector<EntityID> enemyObjects;
        std::vector<EntityID> environmentObjects;
        int nearestID = -1;

    };

}
#endif
