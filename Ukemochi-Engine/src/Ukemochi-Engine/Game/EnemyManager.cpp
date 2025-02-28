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
                //get enemyobj
                GameObject* object = GameObjectManager::GetInstance().GetGO(*it);

    //if no player, enemy wont work
                if (!playerObj)
                    break;

                //skip non active enemy
                if (object->GetActive() == false)
                {
                    it++;
                    continue;
                }

                //init components
                auto& enemycomponent = object->GetComponent<Enemy>();
                auto& enemyphysic = object->GetComponent<Rigidbody2D>();
                auto& enemytransform = object->GetComponent<Transform>();
                auto& sr = object->GetComponent<SpriteRender>();
    //WAIT TIME HANDLE
                if (enemycomponent.iswaiting)
                {
                    if (enemycomponent.waitTime > 0.f)
                    {
                        enemycomponent.waitTime -= static_cast<float>(g_FrameRateController.GetDeltaTime());
                    }
                    else
                    { 

                        if (enemycomponent.state == Enemy::MOVE && enemycomponent.iswaiting)
                        {
                            enemycomponent.state = Enemy::ATTACK;
                            enemycomponent.iswaiting = false;
                            enemycomponent.waitTime = 0.f;
                            enemycomponent.atktimer = 0.f;
                        }
                        if (enemycomponent.state == Enemy::ATTACK && enemycomponent.iswaiting)
                        {
                            enemycomponent.state = Enemy::MOVE;
                            enemycomponent.iswaiting = false;
                            enemycomponent.waitTime = 0.f;
                        }
                    }
                }

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

    //PLAYER KICK ENEMY

                if (enemycomponent.isKick)
                {
                    if (enemycomponent.kicktime > 0.f) {
                        enemycomponent.kicktime -= static_cast<float>(g_FrameRateController.GetDeltaTime());
                    }
                    else {
                        enemycomponent.kicktime = 0.f;
                        enemycomponent.isKick = false;
                    }

                    //if timer reach 1s and no collision is detected enemy is back
                    if (enemycomponent.kicktime <= 1.f && !enemycomponent.isCollide)
                    {
                        enemycomponent.kicktime = 0.f;
                        enemycomponent.isKick = false;
                    }

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
                    //WHEN COLLIDE TO WALL
                            if (collidedObj->GetTag() == "Boundary")
                            {
                                enemycomponent.dirX = enemytransform.position.x - collidedObj->GetComponent<Transform>().position.x;
                                enemycomponent.dirY = enemytransform.position.y - collidedObj->GetComponent<Transform>().position.y;

                                enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                                enemyphysic.force.y = enemycomponent.dirY * enemycomponent.speed;

                                if (enemycomponent.timeSinceTargetReached > 3.0f)
                                {
                                    enemyphysic.force.x = 0;
                                    enemyphysic.force.y = 0;
                                    if (enemycomponent.timeSinceTargetReached > 4.0f)
                                    {
                                        enemycomponent.state = Enemy::MOVE;
                                        enemycomponent.isCollide = false;
                                        enemycomponent.timeSinceTargetReached = 0.f;
                                    }
                                }
                            }
                    //WHEN COLLIDE BOX
                            if (collidedObj->GetTag() == "Environment")
                            {
                                enemycomponent.dirX = collidedObj->GetComponent<Transform>().position.x - enemytransform.position.x;
                                enemycomponent.dirY = collidedObj->GetComponent<Transform>().position.y - enemytransform.position.y;

                                enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                                enemyphysic.force.y = enemycomponent.dirY * enemycomponent.speed;

                                //break boxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
                                if (enemycomponent.timeSinceTargetReached > 3.0f)
                                {
                                    enemyphysic.force.x = 0;
                                    enemyphysic.force.y = 0;
                                    if (enemycomponent.timeSinceTargetReached > 4.0f)
                                    {
                                        enemycomponent.state = Enemy::MOVE;
                                        enemycomponent.isCollide = false;
                                        enemycomponent.timeSinceTargetReached = 0.f;
                                    }
                                }
                            }
                        }
                    }

                    ++it;
                    continue;
                }

    //ENEMY COLLISION RESPONSE
                if (enemycomponent.isCollide)
                {
                    // Start the timer for 1 second if not already running //////////////////////////////////REMOVE THIS
                    if (enemycomponent.timeSinceTargetReached < 1.0f) {
                        enemycomponent.timeSinceTargetReached += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
                    }
                    else {

                        // Timer has reached 1 second, perform the object updates
                        auto* collidedObj = GameObjectManager::GetInstance().GetGO(enemycomponent.collideObj);
                        if (collidedObj->GetTag() != "Environment")
                        {
                            enemycomponent.state = Enemy::MOVE;
                            enemycomponent.isCollide = false;
                            enemycomponent.timeSinceTargetReached = 0.f;
                        }
                        if (collidedObj == nullptr)
                            break;
    //ENEMY TO COLLIDE OBJ
                        float enemyX = enemytransform.position.x;
                        float enemyY = enemytransform.position.y;
                        float objX = collidedObj->GetComponent<Transform>().position.x;
                        float objY = collidedObj->GetComponent<Transform>().position.y;
                        float objWidth = collidedObj->GetComponent<Transform>().scale.x * 0.5f;
                        float objHeight = collidedObj->GetComponent<Transform>().scale.y * 0.5f;

                        // Calculate collision direction
                        float deltaX = enemyX - objX;
                        float deltaY = enemyY - objY;
                        float absDeltaX = std::abs(deltaX);
                        float absDeltaY = std::abs(deltaY);

                        // If enemy has line of sight to the player, reset its movement state
                        if (HasClearLineOfSight(object, playerObj->GetComponent<Transform>()))
                        {
                            enemycomponent.state = Enemy::MOVE;
                            enemycomponent.isCollide = false;
                            enemycomponent.timeSinceTargetReached = 0.f;
                            break;
                        }

                        // Collision handling
                        if (absDeltaX > absDeltaY)  // More horizontal collision
                        {
                            enemyphysic.force.x = 0;  // Stop horizontal movement

                            if (deltaY > 0)  // Enemy is on top of the object
                            {
                                if (!IsObstacleInFront(enemyX, enemyY, 1.0f, 0, 1000.0f)) // Move right if possible
                                {
                                    enemycomponent.dirX = 1.0f;
                                    enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                                }
                                else if (!IsObstacleInFront(enemyX, enemyY, -1.0f, 0, 1000.0f)) // Move left if possible
                                {
                                    enemycomponent.dirX = -1.0f;
                                    enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                                }
                                else // Both left and right blocked, force downward movement
                                {
                                    enemyphysic.force.y = enemycomponent.speed;  // Move up
                                }
                            }
                            else if (deltaY < 0)  // Enemy is at the bottom of the object
                            {
                                if (!IsObstacleInFront(enemyX, enemyY, 1.0f, 0, 1000.0f)) // Move right if possible
                                {
                                    enemycomponent.dirX = 1.0f;
                                    enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                                }
                                else if (!IsObstacleInFront(enemyX, enemyY, -1.0f, 0, 1000.0f)) // Move left if possible
                                {
                                    enemycomponent.dirX = -1.0f;
                                    enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                                }
                                else // Both left and right blocked, force down movement
                                {
                                    enemyphysic.force.y = -enemycomponent.speed;  // Move down
                                }
                            }
                        }
                        else  // More vertical collision
                        {
                            enemyphysic.force.y = 0;  // Stop vertical movement

                            if (enemytransform.position.y < collidedObj->GetComponent<Transform>().position.y - 0.5f * collidedObj->GetComponent<Transform>().scale.y ||
                                enemytransform.position.y > collidedObj->GetComponent<Transform>().position.y + 0.5f * collidedObj->GetComponent<Transform>().scale.y)
                            {
                                enemycomponent.state = Enemy::MOVE;
                                enemycomponent.isCollide = false;
                                enemycomponent.timeSinceTargetReached = 0.f;
                                break;
                            }

                            if (deltaX < 0)  // Enemy is on the left side of the object
                            {
                                if (!IsObstacleInFront(enemyX, enemyY, 0, 1.0f, 1000.0f)) // Move up if possible
                                {
                                    enemyphysic.force.y = enemycomponent.speed;
                                }
                                else if (!IsObstacleInFront(enemyX, enemyY, 0, -1.0f, 1000.0f)) // Move down if possible
                                {
                                    enemyphysic.force.y = -enemycomponent.speed;
                                }
                                else // Both up and down blocked, force movement to the left
                                {
                                    enemycomponent.dirX = -1.0f;
                                    enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                                }
                            }
                            else if (deltaX > 0)  // Enemy is on the right side of the object
                            {
                                if (!IsObstacleInFront(enemyX, enemyY, 0, 1.0f, 1000.0f)) // Move up if possible
                                {
                                    enemyphysic.force.y = enemycomponent.speed;
                                }
                                else if (!IsObstacleInFront(enemyX, enemyY, 0, -1.0f, 1000.0f)) // Move down if possible
                                {
                                    enemyphysic.force.y = -enemycomponent.speed;
                                }
                                else // Both up and down blocked, force movement to the right
                                {
                                    enemycomponent.dirX = 1.0f;
                                    enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                                }
                            }
                        }
                    }
                    ++it;
                    continue;
                }

    //ENEMY STATE MACHINE
                switch (enemycomponent.state)
                {
    //MOVE
                    case Enemy::MOVE:

                        // Compute the direction vector to the target (player)
                        enemycomponent.dirX = playerObj->GetComponent<Transform>().position.x - enemytransform.position.x;
                        enemycomponent.dirY = playerObj->GetComponent<Transform>().position.y - enemytransform.position.y;

                        if (enemycomponent.IsPlayerInAttackRange(playerObj->GetComponent<Transform>(),enemytransform) == false)
                        {
                            if (enemycomponent.iswaiting)
                                break;

                            if (object->GetComponent<Animation>().currentClip != "Walk")
                            {
                                object->GetComponent<Animation>().SetAnimation("Walk");
                            }
                            if (enemycomponent.IsPlayerInRange(playerObj->GetComponent<Transform>(), enemytransform) == false)
                            {
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
                                }
                            }
                            else
                            {
                                float distY = playerObj->GetComponent<Transform>().position.y - enemytransform.position.y;
                                if (distY* distY  > 400.f)
                                {
                                    enemycomponent.dirY = distY;
                                    enemyphysic.force.y = enemycomponent.dirY * enemycomponent.speed;
                                }
                            }

                        }
                        else
                        {
                            if (object->GetComponent<Animation>().currentClip != "Idle")
                            {
                                object->GetComponent<Animation>().SetAnimation("Idle");
                            }
                            enemyphysic.velocity.x = 0.0f;
                            enemyphysic.velocity.y = 0.0f;
                            enemyphysic.force.x = enemyphysic.force.y = 0.f;
                            if (!enemycomponent.iswaiting)
                            {
                                enemycomponent.waitTime = 1.0f;
                                enemycomponent.iswaiting = true;
                            }


                        }
                        break;

                    case Enemy::STUN:
                        break;

                    case Enemy::STANDBY:
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
                                    enemycomponent.iswaiting = true;
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
                }
                ++it;
    //END OF FOR LOOP
            }
        }
    }

    bool EnemyManager::HasClearLineOfSight(GameObject* enemy, const Transform& player)
    {
        // Calculate direction without Normalize()
        
        float dx = player.position.x - enemy->GetComponent<Transform>().position.x;
        float dy = player.position.y - enemy->GetComponent<Transform>().position.y;
        float length = std::sqrt(dx * dx + dy * dy);

        // Avoid division by zero
        float dirX = (length != 0) ? dx / length : 0;
        float dirY = (length != 0) ? dy / length : 0;

        for (EntityID objectID : ECS::GetInstance().GetSystem<DungeonManager>()->rooms[ECS::GetInstance().GetSystem<DungeonManager>()->current_room_id].entities)
        {
            if (enemy->GetInstanceID() == objectID)
            {
                continue;
            }

            GameObject* object = GameObjectManager::GetInstance().GetGO(objectID);

            if (object->GetTag() == "Environment" || object->GetTag() == "Boundary")
            {
                float objX = object->GetComponent<Transform>().position.x;
                float objY = object->GetComponent<Transform>().position.y;
                float objWidth = object->GetComponent<Transform>().scale.x;
                float objHeight = object->GetComponent<Transform>().scale.y;

                if (enemy->GetComponent<Enemy>().LineIntersectsRect(enemy->GetComponent<Transform>().position.x, enemy->GetComponent<Transform>().position.y, dirX, dirY, length, objX, objY, objWidth, objHeight))
                {
                    return false; // Obstacle blocks view
                }
            }
        }
        return true;
    }

    bool EnemyManager::IsObstacleInFront(float startX, float startY, float dirX, float dirY, float range)
    {
        // Normalize direction vector (important for consistent checks)
        float length = std::sqrt(dirX * dirX + dirY * dirY);
        if (length == 0) return false; // Avoid division by zero

        float normDirX = dirX / length;
        float normDirY = dirY / length;

        // Compute the endpoint of the check line
        float endX = startX + normDirX * range;
        float endY = startY + normDirY * range;

        for (EntityID objectID : ECS::GetInstance().GetSystem<DungeonManager>()->rooms[ECS::GetInstance().GetSystem<DungeonManager>()->current_room_id].entities)
        {
            GameObject* object = GameObjectManager::GetInstance().GetGO(objectID);
            if (!object) continue;

            if (object->GetTag() == "Environment" || object->GetTag() == "Boundary")
            {
                Transform& objTransform = object->GetComponent<Transform>();
                float objX = objTransform.position.x;
                float objY = objTransform.position.y;
                float objWidth = objTransform.scale.x * 0.5f;  // Half-width for AABB check
                float objHeight = objTransform.scale.y * 0.5f; // Half-height for AABB check

                // Compute bounding box
                float minX = objX - objWidth;
                float maxX = objX + objWidth;
                float minY = objY - objHeight;
                float maxY = objY + objHeight;

                // Compute direction vector
                float dirX = endX - startX;
                float dirY = endY - startY;
                float length = std::sqrt(dirX * dirX + dirY * dirY);

                // Normalize direction vector
                if (length > 0.0f) {
                    dirX /= length;
                    dirY /= length;
                }

                if (Enemy::LineIntersectsRect(startX, startY, dirX, dirY, length, objX, objY, objWidth * 2, objHeight * 2))
                {
                    return true; // Obstacle detected
                }
            }
        }
        return false; // No obstacles found
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
            enemyComponent.timeSinceTargetReached = 1.f;
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
