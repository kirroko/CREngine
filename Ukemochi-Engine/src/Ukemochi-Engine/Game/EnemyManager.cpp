/* Start Header ************************************************************************/
/*!
\file       EnemyManager.h
\author     Tan Si Han, t.sihan, 2301264, t.sihan@digipen.edu
\date       Sept 20, 2024
\brief      This file contains the definition of the EnemyManager class and related methods.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "EnemyManager.h"
#include "../Factory/GameObjectManager.h"
#include "../FrameController.h"
#include "DungeonManager.h"
#include "Ukemochi-Engine/Game/PlayerManager.h" // for player data
#include "SoulManager.h" // for soul harvest

namespace Ukemochi
{
    /*!***********************************************************************
    \brief
        Updates the list of enemies in the game.
    \details
        This method performs any necessary updates on the enemies currently managed by the system.
    *************************************************************************/
	void EnemyManager::UpdateEnemyList()
	{
        std::srand(static_cast<unsigned int>(std::time(nullptr))); // Seed the random generator
        enemyObjects.clear();
        environmentObjects.clear();
        numEnemyTarget = 0;

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

    /*!***********************************************************************
    \brief
        Updates the behavior of all enemies.
    \details
        This method is called to update each enemy's behavior and logic during the game's update phase.
    *************************************************************************/
    void EnemyManager::UpdateEnemies()
    {
        // Update the enemy manager based on the number of steps
        for (int step = 0; step < g_FrameRateController.GetCurrentNumberOfSteps(); ++step)
        {
            // Iterate through the enemyObjects list
            for (auto it = enemyObjects.begin(); it != enemyObjects.end();)
            {
                GameObject* object = GameObjectManager::GetInstance().GetGO(*it);
                object->GetComponent<Animation>().SetAnimation("Idle");

                if (object->GetActive() == false)
                {
                    it++;
                    continue;
                }

                auto& enemycomponent = object->GetComponent<Enemy>();
                auto& enemyphysic = object->GetComponent<Rigidbody2D>();
                auto& enemytransform = object->GetComponent<Transform>();
                auto& sr = object->GetComponent<SpriteRender>();

                if (enemycomponent.health <= 0.f)
                {
                    enemycomponent.state = Enemy::DEAD;
                }
                else
                {
                    object->SetActive(true);
                }

                //animation
                if (enemyphysic.force.x < 0)
                {
                    auto& anim = ECS::GetInstance().GetComponent<Animation>(object->GetInstanceID());
                    //anim.SetAnimation("Running");
                    sr.flipX = false;
                }
                else if (enemyphysic.force.x > 0)
                {
                    sr.flipX = true;
                }


                // If the enemy is in DEAD state, remove it from the list after processing DeadState
                if (enemycomponent.state == Enemy::DEAD && !enemycomponent.isDead)
                {
                    auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                    //dont overlap kick sound
                    if (audioM.GetSFXindex("Pattack3") != -1 && audioM.GetSFXindex("EnemyKilled") != -1)
                    {
                        if ((!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("Pattack3"))) && !enemycomponent.isDead)
                        {
                            audioM.PlaySFX(audioM.GetSFXindex("EnemyKilled"));
                        }
                    }

                    // Harvest the soul of the dead enemy
                    ECS::GetInstance().GetSystem<SoulManager>()->HarvestSoul(static_cast<SoulType>(enemycomponent.type), 1);

                    object->SetActive(false);
                    enemycomponent.isDead = true;
                    if (enemycomponent.isWithPlayer && numEnemyTarget >= 1)
                    {
                        numEnemyTarget--;
                    }
                    ++it;
                    continue;
                }

                //if there no nearest obj, find neareast obj
                if (enemycomponent.nearestObj == -1)
                {
                    enemycomponent.nearestObj = FindNearestObject(object);
                }

                if (enemycomponent.state != enemycomponent.ATTACK)
                {
                    auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                    if (enemycomponent.type == enemycomponent.FISH && audioM.GetSFXindex("FishMove") != -1)
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
                if (enemycomponent.isKick)
                {
                    if (enemycomponent.timeSinceTargetReached < 1.0f) {
                        enemycomponent.timeSinceTargetReached += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
                    }
                    else {
                        enemycomponent.timeSinceTargetReached = 0.f;
                        enemycomponent.isKick = false;
                    }
                    ++it;
                    continue;
                }

                // Skip collision handling for dead enemies
                if (enemycomponent.isCollide)
                {
                    // Start the timer for 1 second if not already running
                    if (enemycomponent.timeSinceTargetReached < 1.0f) {
                        enemycomponent.timeSinceTargetReached += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
                    }
                    else {

                        // Timer has reached 1 second, perform the object updates
                        enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                        enemyphysic.force.y = -enemycomponent.dirY * enemycomponent.speed;

                        if (IsEnemyAwayFromObject(object, GameObjectManager::GetInstance().GetGO(enemycomponent.nearestObj), 300.f) && enemycomponent.state == enemycomponent.ROAM)
                        {
                            enemycomponent.prevObject2 = enemycomponent.prevObject;
                            enemycomponent.prevObject = enemycomponent.nearestObj;
                            enemycomponent.nearestObj = -1;

                            if (enemycomponent.nearestObj == enemycomponent.prevObject2 && enemycomponent.nearestObj == enemycomponent.prevObject)
                            {
                                enemycomponent.nearestObj = FindNearestObject(object);
                            }
                            else
                            {
                                enemycomponent.isCollide = false;
                                enemycomponent.timeSinceTargetReached = 0.f;
                            }

                        }
                    }

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

                        if (enemycomponent.ReachedTarget(enemyphysic.position.x, enemyphysic.position.y, enemycomponent.targetX, enemycomponent.targetY, 20.f))
                        {
                            enemyphysic.velocity.x = 0.0f;
                            enemyphysic.velocity.y = 0.0f;
                            enemyphysic.force.x = enemyphysic.force.y = 0.f;
                            // Start the timer for 1 second if not already running
                            if (enemycomponent.timeSinceTargetReached < 1.0f) {
                                enemycomponent.timeSinceTargetReached += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
                            }
                            else {
                                // Timer has reached 1 second, perform the object updates
                                enemycomponent.prevObject = enemycomponent.nearestObj;
                                enemycomponent.nearestObj = -1;

                                // Reset the timer after execution
                                enemycomponent.timeSinceTargetReached = 0.0f;
                            }
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
                    break;

                case Enemy::CHASE:
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
                        if (enemycomponent.IsPlayerInRange(playerObj->GetComponent<Transform>(), enemytransform))
                        {
                            // Stop movement and transition to ATTACK state
                            enemyphysic.velocity.x = 0.0f;
                            enemyphysic.velocity.y = 0.0f;
                            enemyphysic.force.x = enemyphysic.force.y = 0.f;
                            enemycomponent.state = enemycomponent.STANDBY;
                            break;
                        }
                    }
                    else
                    {
                        // If the magnitude is zero (enemy is on top of the player), go to ATTACK
                        enemyphysic.velocity.x = 0.0f;
                        enemyphysic.velocity.y = 0.0f;
                        enemyphysic.force.x = enemyphysic.force.y = 0.f;
                        enemycomponent.state = enemycomponent.STANDBY;
                        break;
                    }

                    // Ensure other transitions (like to ROAM) don�t block the ATTACK state
                    if (IsEnemyAwayFromObject(object, playerObj, 350.0f))
                    {
                        //std::cout << "Transitioning to ROAM state for enemy: " << object->GetInstanceID() << std::endl;
                        enemycomponent.state = enemycomponent.ROAM;
                        break;
                    }
                    break;

                case Enemy::STANDBY:
                    if (numEnemyTarget < 1)
                    {
                        numEnemyTarget++;
                        enemycomponent.state = enemycomponent.ATTACK;
                    }

                    // Ensure other transitions (like to ROAM) don�t block the ATTACK state
                    if (IsEnemyAwayFromObject(object, playerObj, 350.0f))
                    {
                        //std::cout << "Transitioning to ROAM state for enemy: " << object->GetInstanceID() << std::endl;
                        enemycomponent.state = enemycomponent.ROAM;
                        break;
                    }
                    break;

                case Enemy::ATTACK:

                    if (!enemycomponent.IsPlayerInRange(playerObj->GetComponent<Transform>(), enemytransform))
                    {
                        enemycomponent.state = enemycomponent.CHASE;
                        break;
                    }

                    enemycomponent.atktimer -= static_cast<float>(g_FrameRateController.GetFixedDeltaTime());

                    if (enemycomponent.atktimer <= 1.0f)
                    {
                        //Charge attack for fish
                        //shoot for worm
                        static bool attack = false;
                        auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                        if (enemycomponent.type == enemycomponent.FISH && audioM.GetSFXindex("FishAttack") != -1)
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
    }

    /*!***********************************************************************
    \brief
        Handles the collision response when an enemy collides with another object.
    \param enemyID: The ID of the enemy involved in the collision.
    \param objID: The ID of the object involved in the collision.
    \details
        This method processes the appropriate response when an enemy collides with another entity,
        which could include interactions with the player or other objects.
    *************************************************************************/
    void EnemyManager::EnemyCollisionResponse(EntityID enemyID, EntityID objID)
    {
        auto& gameObjectManager = GameObjectManager::GetInstance();
        auto enemy = gameObjectManager.GetGO(enemyID);
        auto obj2 = gameObjectManager.GetGO(objID);

        auto& enemyComponent = enemy->GetComponent<Enemy>();

        if (obj2->HasComponent<Enemy>()) //NOT IN USED
        {
            auto& enemyComponent2 = obj2->GetComponent<Enemy>();
            enemyComponent2.isCollide = false;
            enemyComponent.isCollide = false;
        }
        else
        {
            //set collide to true then now the obj is the obj save the pathfinding obj as prev
            enemyComponent.isCollide = true;
            enemyComponent.timeSinceTargetReached = 0.f;
            //enemyComponent.prevObject = static_cast<int>(enemyComponent.nearestObj);
            //enemyComponent.nearestObj = static_cast<int>(objID);
        }
    }

    /*!***********************************************************************
    \brief
        Determines if there is a clear path for an enemy to move to a new position.
    \param enemy: Pointer to the GameObject representing the enemy.
    \param newX: The target X coordinate for the enemy to move to.
    \param newY: The target Y coordinate for the enemy to move to.
    \return
        Returns true if the enemy can move to the new position without obstruction.
    *************************************************************************/
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

    /*!***********************************************************************
    \brief
        Finds the nearest object to a given enemy.
    \param enemy: Pointer to the GameObject representing the enemy.
    \return
        The ID of the nearest object to the enemy.
    *************************************************************************/
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

    /*!***********************************************************************
    \brief
        Checks if an enemy is far enough away from a target object.
    \param enemy: Pointer to the GameObject representing the enemy.
    \param targetObject: Pointer to the GameObject representing the target.
    \param minDistanceThreshold: The minimum distance threshold to check.
    \return
        Returns true if the enemy is farther than the given threshold from the target object.
    *************************************************************************/
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

    /*!***********************************************************************
    \brief
        Clears all enemy entities.
    \details
        This method removes all enemies from the enemy list and performs any necessary cleanup.
    *************************************************************************/
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
