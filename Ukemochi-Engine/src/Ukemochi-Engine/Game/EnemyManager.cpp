/* Start Header ************************************************************************/
/*!
\file       EnemyManager.h
\author     Tan Si Han, t.sihan, 2301264, t.sihan@digipen.edu (90%)
\co-authors HURNG Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu (10%)
\date       Feb 05, 2024
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
        for (int step = 0; step < g_FrameRateController.GetCurrentNumberOfSteps(); ++step)
        {

            // Iterate through the enemyObjects list
            for (auto it = enemyObjects.begin(); it != enemyObjects.end();)
            {
                GameObject* object = GameObjectManager::GetInstance().GetGO(*it);
                //if (object->GetComponent<Animation>().currentClip != "Idle")
                //{
                //    object->GetComponent<Animation>().SetAnimation("Idle");
                //}

                if (object->GetActive() == false)
                {
                    it++;
                    continue;
                }

                auto& enemycomponent = object->GetComponent<Enemy>();
                auto& enemyphysic = object->GetComponent<Rigidbody2D>();
                auto& enemytransform = object->GetComponent<Transform>();
                auto& sr = object->GetComponent<SpriteRender>();

                // Handle enemy hit state and sound effects
                if (enemycomponent.wasHit)
                {
                    // Only play hit sound if this hit wasn't fatal (health > 0)
                    if (enemycomponent.health > 0)
                    {
                        auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                        // Play different hit sounds based on enemy type
                        if (enemycomponent.type == Enemy::FISH && audioM.GetSFXindex("FishHurt") != -1)
                        {
                            audioM.PlaySFX(audioM.GetSFXindex("FishHurt"));
                        }
                        else if (enemycomponent.type == Enemy::WORM && audioM.GetSFXindex("WormHurt") != -1)
                        {
                            if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("WormHurt")))
                            {
                                audioM.PlaySFX(audioM.GetSFXindex("WormHurt"));
                            }
                        }
                    }
                    enemycomponent.wasHit = false; // Reset the hit flag
                }


                // When enemy health reaches 0
                if (enemycomponent.health <= 0.f && !enemycomponent.isDead)  // Add !isDead check
                {
                    // Play death sound immediately when health hits 0
                    auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                    if (audioM.GetSFXindex("EnemyKilled") != -1)
                    {
                        audioM.PlaySFX(audioM.GetSFXindex("EnemyKilled"));
                    }
                    enemycomponent.state = Enemy::DEAD;
                    enemycomponent.isDead = true;  // Mark as dead immediately
                }
                else
                {
                    object->SetActive(true);
                }

                // Animation and sound synchronization
                auto& anim = object->GetComponent<Animation>();
                if (anim.currentClip == "Walk")
                {
                    if (enemycomponent.type == Enemy::FISH)
                    {
                        // Play FishMove sound at specific frames
                        if (anim.GetCurrentFrame() == 2 || anim.GetCurrentFrame() == 6)
                        {
                            auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                            if (audioM.GetSFXindex("FishMove") != -1)
                            {
                                if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("FishMove")))
                                {
                                    audioM.PlaySFX(audioM.GetSFXindex("FishMove"));
                                }
                            }
                        }
                    }
                    if (enemycomponent.type == Enemy::WORM)
                    {
                        // Play FishMove sound at specific frames
                        if (anim.GetCurrentFrame() == 2 || anim.GetCurrentFrame() == 6)
                        {
                            auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                            if (audioM.GetSFXindex("WormMove") != -1)
                            {
                                if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("WormMove")))
                                {
                                    audioM.PlaySFX(audioM.GetSFXindex("WormMove"));
                                }
                            }
                        }
                    }
                }

                //animation
                if (enemycomponent.dirX < 0)
                {
                    //auto& anim = ECS::GetInstance().GetComponent<Animation>(object->GetInstanceID());
                    //anim.SetAnimation("Running");
                    sr.flipX = false;
                }
                else if (enemycomponent.dirX > 0)
                {
                    sr.flipX = true;
                }


                // If the enemy is in DEAD state, remove it from the list after processing DeadState
                if (enemycomponent.state == Enemy::DEAD)
                {
                    if (object->GetComponent<Animation>().currentClip != "Death")
                    {
                        object->GetComponent<Animation>().SetAnimation("Death");
                    }

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
                    ECS::GetInstance().GetSystem<SoulManager>()->HarvestSoul(static_cast<SoulType>(enemycomponent.type), 20.f);

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

                //if (enemycomponent.state != enemycomponent.ATTACK)
                //{
                //    bool isMoving = (std::abs(enemyphysic.force.x) > 0.1f || std::abs(enemyphysic.force.y) > 0.1f);
                //}

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
                        enemycomponent.timeSinceTargetReached += static_cast<float>(g_FrameRateController.GetDeltaTime());
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
                        auto* collidedObj = GameObjectManager::GetInstance().GetGO(enemycomponent.collideObj);
                        if (collidedObj == nullptr)
                            break;
                        if (collidedObj->GetTag() == "Boundary")
                        {
                            enemyphysic.force.x = -enemycomponent.dirX * enemycomponent.speed;
                            enemyphysic.force.y = -enemycomponent.dirY * enemycomponent.speed;
                            enemycomponent.timeSinceTargetReached += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());

                            if (enemycomponent.timeSinceTargetReached > 3.0f)
                            {
                                enemycomponent.prevObject2 = enemycomponent.prevObject;
                                enemycomponent.prevObject = enemycomponent.nearestObj;
                                enemycomponent.nearestObj = -1;
                                enemycomponent.collideObj = -1;

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
                        else
                        {
                            auto& collidedTransform = ECS::GetInstance().GetComponent<Transform>(enemycomponent.collideObj);
                            Vec2 awayDir;
                            Vec2Normalize(awayDir, Vec2(enemytransform.position.x - collidedTransform.position.x,
                                enemytransform.position.y - collidedTransform.position.y));

                            enemycomponent.dirX = awayDir.x;
                            enemycomponent.dirY = awayDir.y;
                            enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                            enemyphysic.force.y = enemycomponent.dirY * enemycomponent.speed;

                            if (IsEnemyAwayFromObject(object, GameObjectManager::GetInstance().GetGO(enemycomponent.collideObj), 300.f) && enemycomponent.state == enemycomponent.ROAM)
                            {
                                enemycomponent.prevObject2 = enemycomponent.prevObject;
                                enemycomponent.prevObject = enemycomponent.nearestObj;
                                enemycomponent.nearestObj = -1;
                                enemycomponent.collideObj = -1;

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

                        //enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                        //enemyphysic.force.y = -enemycomponent.dirY * enemycomponent.speed;

                    }

                    ++it;
                    continue;
                }


                // Handle other states
                switch (enemycomponent.state)
                {
                case Enemy::ROAM:
                    if (object->GetComponent<Animation>().currentClip != "Walk")
                    {
                        object->GetComponent<Animation>().SetAnimation("Walk");
                    }
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
                    if (object->GetComponent<Animation>().currentClip != "Walk")
                    {
                        object->GetComponent<Animation>().SetAnimation("Walk");
                    }
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
                            if (enemycomponent.type == enemycomponent.FISH)
                            {
                                if (std::abs(playerObj->GetComponent<Transform>().position.x - enemytransform.position.x) < 150.f)
                                {
                                    enemyphysic.velocity.x = 0.0f;
                                    enemyphysic.velocity.y = 0.0f;
                                    enemyphysic.force.x = enemyphysic.force.y = 0.f;
                                    enemycomponent.state = enemycomponent.STANDBY;
                                }
                            }
                            if (enemycomponent.type == enemycomponent.WORM)
                            {
                                if (std::abs(playerObj->GetComponent<Transform>().position.x - enemytransform.position.x) < 3000.f)
                                {
                                    enemyphysic.velocity.x = 0.0f;
                                    enemyphysic.velocity.y = 0.0f;
                                    enemyphysic.force.x = enemyphysic.force.y = 0.f;
                                    enemycomponent.state = enemycomponent.STANDBY;
                                }
                            }
                            // Stop movement and transition to ATTACK state
                            //enemyphysic.velocity.x = 0.0f;
                            //enemyphysic.velocity.y = 0.0f;
                            //enemyphysic.force.x = enemyphysic.force.y = 0.f;

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
                    if (object->GetComponent<Animation>().currentClip != "Idle")
                    {
                        object->GetComponent<Animation>().SetAnimation("Idle");
                    }
                    if (numEnemyTarget < 2)
                    {
                        if (!enemycomponent.isWithPlayer)
                        {
                            numEnemyTarget++;
                            enemycomponent.state = enemycomponent.ATTACK;
                            enemycomponent.isWithPlayer = true;
                        }

                    }

                    // Ensure other transitions (like to ROAM) don�t block the ATTACK state
                    if (IsEnemyAwayFromObject(object, playerObj, 350.0f))
                    {
                        //std::cout << "Transitioning to ROAM state for enemy: " << object->GetInstanceID() << std::endl;
                        if (enemycomponent.isWithPlayer)
                        {
                            numEnemyTarget--;
                            enemycomponent.state = enemycomponent.ROAM;
                            enemycomponent.isWithPlayer = false;
                        }
                        break;
                    }
                    break;

                case Enemy::ATTACK:

                    enemycomponent.atktimer -= static_cast<float>(g_FrameRateController.GetDeltaTime());

                    if (enemycomponent.atktimer < 0)
                    {
                        if (object->GetComponent<Animation>().currentClip != "Attack")
                        {
                            object->GetComponent<Animation>().SetAnimation("Attack");
                        }
                    }
                    else
                    {
                        if (object->GetComponent<Animation>().currentClip != "Idle")
                        {
                            object->GetComponent<Animation>().SetAnimation("Idle");
                        }
                    }

                    //Charge attack for fish
                    if (enemycomponent.type == enemycomponent.FISH)
                    {

                        if (object->GetComponent<Animation>().currentClip == "Attack" && object->GetComponent<Animation>().current_frame == 18)
                        {
                            //SFX
                            auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                            if (audioM.GetSFXindex("FishAttack") != -1)
                            {
                                if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("FishAttack")))
                                {
                                    audioM.PlaySFX(audioM.GetSFXindex("FishAttack"));

                                    //check attack
                                    if (enemycomponent.IsPlayerInRange(playerObj->GetComponent<Transform>(), enemytransform))
                                    {
                                        //attack
                                        if (playerObj != nullptr)
                                        {
                                            enemycomponent.AttackPlayer(playerObj->GetComponent<Player>().currentHealth);
                                            ECS::GetInstance().GetSystem<PlayerManager>()->OnCollisionEnter(playerObj->GetInstanceID());
                                        }
                                    }
                                }

                            }

                        }

                        if (object->GetComponent<Animation>().currentClip == "Attack" && object->GetComponent<Animation>().current_frame == 22)
                        {
                            if (!enemycomponent.IsPlayerInRange(playerObj->GetComponent<Transform>(), enemytransform))
                            {
                                enemycomponent.state = enemycomponent.STANDBY;
                                enemycomponent.atktimer = 0.f;
                                break;
                            }
                            else
                            {
                                enemycomponent.atktimer = 1.5f;
                            }

                        }
                    }
                    else if (enemycomponent.type == enemycomponent.WORM)
                    {
                        if (object->GetComponent<Animation>().currentClip == "Attack" && object->GetComponent<Animation>().current_frame == 19)
                        {
                            if (!enemycomponent.IsPlayerInRange(playerObj->GetComponent<Transform>(), enemytransform))
                            {
                                enemycomponent.state = enemycomponent.STANDBY;
                                enemycomponent.atktimer = 0.f;
                                enemycomponent.wormshoot = false;
                                break;
                            }
                            else
                            {
                                enemycomponent.atktimer = 1.5f;
                                enemycomponent.wormshoot = false;
                            }

                        }

                        if (object->GetComponent<Animation>().currentClip == "Attack" && object->GetComponent<Animation>().current_frame == 15 && !enemycomponent.wormshoot)
                        {
                            //Play SFX
                            auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                            if (audioM.GetSFXindex("WormAttack") != -1)
                            {
                                if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("WormAttack")))
                                {
                                    audioM.PlaySFX(audioM.GetSFXindex("WormAttack"));
                                }

                            }

                            //spawn bullet
                            static int number = 0;
                            GameObject* cloneObject = GameObjectManager::GetInstance().GetGOByTag("EnemyProjectile1");

                            if (cloneObject != nullptr)
                            {
                                std::string name = "bullet" + std::to_string(number++);
                                GameObject& newObject = GameObjectManager::GetInstance().CloneObject(*cloneObject, name, "EnemyProjectile");

                                newObject.GetComponent<Transform>().position.x = enemytransform.position.x;
                                newObject.GetComponent<Transform>().position.y = enemytransform.position.y + 50.f;

                                newObject.GetComponent<Animation>().SetAnimation("Projectile");

                                auto playerpos = playerObj->GetComponent<Transform>().position;

                                Vec2 dir;
                                Vec2Normalize(dir, Vec2(playerpos.x - newObject.GetComponent<Transform>().position.x,
                                    playerpos.y - newObject.GetComponent<Transform>().position.y));

                                newObject.GetComponent<Rigidbody2D>().velocity = dir * 500;

                                float angleRad = atan2(dir.y, dir.x);

                                // Convert to degrees
                                float angleDeg = angleRad * (180.0f / 3.14159265358979323846f);

                                if (newObject.GetComponent<Rigidbody2D>().velocity.x > 0)
                                {
                                    newObject.GetComponent<SpriteRender>().flipX = true;
                                    // Apply rotation to bullet
                                    newObject.GetComponent<Transform>().rotation = angleDeg;

                                }
                                else if (newObject.GetComponent<Rigidbody2D>().velocity.x > 0)
                                {
                                    newObject.GetComponent<SpriteRender>().flipX = false;
                                    // Apply rotation to bullet
                                    newObject.GetComponent<Transform>().rotation = -angleDeg;
                                }

                                newObject.AddComponent(EnemyBullet{});
                                enemycomponent.wormshoot = true;
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
            enemyComponent.collideObj = static_cast<int>(objID);
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
