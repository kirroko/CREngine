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
#include "SoulManager.h"                        // for soul harvest

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
            GameObject *object = GameObjectManager::GetInstance().GetGO(objectID);
            if (object->HasComponent<Enemy>())
            {
                enemyObjects.push_back(object->GetInstanceID());
            }
        }
        for (EntityID objectID : ECS::GetInstance().GetSystem<DungeonManager>()->rooms[ECS::GetInstance().GetSystem<DungeonManager>()->current_room_id].entities)
        {
            GameObject *object = GameObjectManager::GetInstance().GetGO(objectID);
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
        GameObject *player = GameObjectManager::GetInstance().GetGOByTag("Player");
        auto &playerTransform = player->GetComponent<Transform>();
        float soundRange = 750.0f; // Maximum distance for sound to play

        for (int step = 0; step < g_FrameRateController.GetCurrentNumberOfSteps(); ++step)
        {
            // Iterate through the enemyObjects list
            for (auto it = enemyObjects.begin(); it != enemyObjects.end();)
            {
                // get enemyobj
                GameObject *object = GameObjectManager::GetInstance().GetGO(*it);

                // if no player, enemy wont work
                if (!playerObj)
                    break;

                // skip non active enemy
                if (object->GetActive() == false)
                {
                    it++;
                    continue;
                }

                // init components
                auto &enemycomponent = object->GetComponent<Enemy>();
                auto &enemyphysic = object->GetComponent<Rigidbody2D>();
                auto &enemytransform = object->GetComponent<Transform>();
                auto &sr = object->GetComponent<SpriteRender>();

                // Check if the enemy is within range of the player
                float deltaX = playerTransform.position.x - enemytransform.position.x;
                float deltaY = playerTransform.position.y - enemytransform.position.y;

                // Euclidean distance (Pythagorean theorem)
                float distance = sqrt(deltaX * deltaX + deltaY * deltaY);

                // WAIT TIME HANDLE
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
                            enemycomponent.state = Enemy::STANDBY;
                            enemycomponent.iswaiting = false;
                            enemycomponent.waitTime = 0.f;
                            enemycomponent.atktimer = 0.f;
                        }
                        if (enemycomponent.state == Enemy::STANDBY && enemycomponent.iswaiting)
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
                        auto &audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                        // Play different hit sounds based on enemy type
                        if (enemycomponent.type == Enemy::FISH)
                        {
                            std::vector<int> fishHurtSounds;
                            // Add all available fish hurt sounds to the array
                            int fishHurt1 = audioM.GetSFXindex("FishHurt1");
                            if (fishHurt1 != -1)
                                fishHurtSounds.push_back(fishHurt1);

                            int fishHurt2 = audioM.GetSFXindex("FishHurt2");
                            if (fishHurt2 != -1)
                                fishHurtSounds.push_back(fishHurt2);

                            int fishHurt3 = audioM.GetSFXindex("FishHurt3");
                            if (fishHurt3 != -1)
                                fishHurtSounds.push_back(fishHurt3);

                            int fishHurt4 = audioM.GetSFXindex("FishHurt4");
                            if (fishHurt4 != -1)
                                fishHurtSounds.push_back(fishHurt4);

                            // Original sound as fallback
                            int fishHurtOriginal = audioM.GetSFXindex("FishHurt1");
                            if (fishHurtOriginal != -1)
                                fishHurtSounds.push_back(fishHurtOriginal);

                            // If we have fish hurt sounds available, play a random one
                            if (!fishHurtSounds.empty())
                            {
                                // Generate a random index into our array of sounds
                                int randomIndex = rand() % fishHurtSounds.size();
                                audioM.PlaySFX(fishHurtSounds[randomIndex]);
                            }
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
                if (enemycomponent.health <= 0.f && !enemycomponent.isDead) // Add !isDead check
                {
                    // Play death sound immediately when health hits 0
                    auto &audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();

                    // Play different death sounds based on enemy type
                    if (enemycomponent.type == Enemy::FISH)
                    {
                        // Create an array of available fish death sound indices
                        std::vector<int> fishDeadSounds;

                        // Add all available fish death sounds to the array
                        int fishDead1 = audioM.GetSFXindex("FishDead1");
                        if (fishDead1 != -1)
                            fishDeadSounds.push_back(fishDead1);

                        int fishDead2 = audioM.GetSFXindex("FishDead2");
                        if (fishDead2 != -1)
                            fishDeadSounds.push_back(fishDead2);

                        int fishDead3 = audioM.GetSFXindex("FishDead3");
                        if (fishDead3 != -1)
                            fishDeadSounds.push_back(fishDead3);

                        // If we have fish death sounds available, play a random one
                        if (!fishDeadSounds.empty())
                        {
                            // Generate a random index into our array of sounds
                            int randomIndex = rand() % fishDeadSounds.size();
                            audioM.PlaySFX(fishDeadSounds[randomIndex]);
                        }
                    }
                    else if (enemycomponent.type == Enemy::WORM)
                    {
                        // Use a specific worm death sound if available
                        int wormDead = audioM.GetSFXindex("WormDead");
                        if (wormDead != -1)
                        {
                            audioM.PlaySFX(wormDead);
                        }
                        // If WormDead sound isn't available, fall back to generic death sound
                        else if (audioM.GetSFXindex("EnemyKilled") != -1)
                        {
                            audioM.PlaySFX(audioM.GetSFXindex("EnemyKilled"));
                        }
                    }
                    else
                    {
                        // For any other enemy types, use the generic death sound
                        if (audioM.GetSFXindex("EnemyKilled") != -1)
                        {
                            audioM.PlaySFX(audioM.GetSFXindex("EnemyKilled"));
                        }
                    }

                    enemycomponent.state = Enemy::DEAD;
                    enemycomponent.isDead = true; // Mark as dead immediately
                }
                else
                {
                    object->SetActive(true);
                }

                // Animation and sound synchronization
                auto &anim = object->GetComponent<Animation>();
                if (anim.currentClip == "Walk" && distance <= soundRange)
                {
                    if (enemycomponent.type == Enemy::FISH)
                    {
                        // Play FishMove sound at specific frames
                        if (anim.GetCurrentFrame() == 2 || anim.GetCurrentFrame() == 6)
                        {
                            auto &audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                            if (audioM.GetSFXindex("FishMove") != -1)
                            {
                                if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("FishMove")))
                                {
                                    audioM.PlaySFX(audioM.GetSFXindex("FishMove"));
                                }
                            }
                        }
                    }
                    if (enemycomponent.type == Enemy::WORM && distance <= soundRange)
                    {
                        // Play FishMove sound at specific frames
                        if (anim.GetCurrentFrame() == 2 || anim.GetCurrentFrame() == 6)
                        {
                            auto &audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
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

                // animation
                if (enemycomponent.dirX < 0)
                {
                    // auto& anim = ECS::GetInstance().GetComponent<Animation>(object->GetInstanceID());
                    // anim.SetAnimation("Running");
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

                    auto &audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                    // dont overlap kick sound
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
                        if (numEnemyTarget < 0)
                        {
                            numEnemyTarget = 0;
                        }
                    }
                    ++it;
                    continue;
                }

                // PLAYER KICK ENEMY

                if (enemycomponent.isKick)
                {
                    if (object->GetComponent<Animation>().currentClip == "Hurt" && object->GetComponent<Animation>().current_frame == 3)
                    {
                        object->GetComponent<Animation>().SetAnimationUninterrupted("Idle");
                    }

                    if (enemycomponent.kicktime > 0.f && !enemycomponent.isCollide)
                    {
                        enemycomponent.kicktime -= static_cast<float>(g_FrameRateController.GetDeltaTime());
                    }
                    else if (enemycomponent.kicktime < 0.f && !enemycomponent.isCollide)
                    {
                        enemycomponent.kicktime = 1.f;
                        enemycomponent.isKick = false;
                    }

                    // if timer reach 1s and no collision is detected enemy is back
                    if (enemycomponent.kicktime <= 0.5f && !enemycomponent.isCollide)
                    {
                        enemycomponent.kicktime = 1.f;
                        enemycomponent.isKick = false;
                    }

                    if (enemycomponent.isCollide)
                    {
                        // Start the timer for 1 second if not already running
                        if (enemycomponent.timeSinceTargetReached < 1.0f)
                        {
                            enemycomponent.timeSinceTargetReached += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
                        }
                        else
                        {
                            // Timer has reached 1 second, perform the object updates
                            auto *collidedObj = GameObjectManager::GetInstance().GetGO(enemycomponent.collideObj);
                            if (collidedObj == nullptr)
                                break;
                            // WHEN COLLIDE TO WALL
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
                                        enemycomponent.kicktime = 1.f;
                                        enemycomponent.isKick = false;
                                        enemycomponent.timeSinceTargetReached = 0.f;
                                    }
                                }
                            }
                            // WHEN COLLIDE BOX
                            if (collidedObj->GetTag() == "Environment")
                            {
                                enemycomponent.dirX = collidedObj->GetComponent<Transform>().position.x - enemytransform.position.x;
                                enemycomponent.dirY = collidedObj->GetComponent<Transform>().position.y - enemytransform.position.y;

                                enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                                enemyphysic.force.y = enemycomponent.dirY * enemycomponent.speed;

                                // break boxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
                                if (enemycomponent.timeSinceTargetReached > 3.0f)
                                {
                                    enemyphysic.force.x = 0;
                                    enemyphysic.force.y = 0;
                                    if (enemycomponent.timeSinceTargetReached > 4.0f)
                                    {
                                        enemycomponent.state = Enemy::MOVE;
                                        enemycomponent.isCollide = false;
                                        enemycomponent.kicktime = 1.f;
                                        enemycomponent.isKick = false;
                                        enemycomponent.timeSinceTargetReached = 0.f;
                                    }
                                }
                            }
                        }
                    }

                    ++it;
                    continue;
                }

                // ENEMY COLLISION RESPONSE
                if (enemycomponent.isCollide)
                {
                    // Start the timer for 1 second if not already running //////////////////////////////////REMOVE THIS
                    if (enemycomponent.timeSinceTargetReached < 1.0f)
                    {
                        enemycomponent.timeSinceTargetReached += static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
                    }
                    else
                    {

                        // Timer has reached 1 second, perform the object updates
                        auto *collidedObj = GameObjectManager::GetInstance().GetGO(enemycomponent.collideObj);
                        if (collidedObj->GetTag() != "Environment")
                        {
                            enemycomponent.state = Enemy::MOVE;
                            enemycomponent.isCollide = false;
                            enemycomponent.timeSinceTargetReached = 0.f;
                        }
                        if (collidedObj == nullptr)
                            break;
                        // ENEMY TO COLLIDE OBJ
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
                            enemycomponent.move = 5;
                            break;
                        }

                        // Collision handling
                        if (enemytransform.position.y > collidedObj->GetComponent<Transform>().position.y + collidedObj->GetComponent<Transform>().scale.x * 0.25f) // top
                        {
                            // std::cout << "ENEMY TOP SIDE" << std::endl;
                            enemyphysic.force.y = 0; // Stop vertical movement

                            // check where player
                            if (playerObj->GetComponent<Transform>().position.x > enemytransform.position.x) // player on right
                            {
                                if (!IsObstacleInFront(enemyX, enemyY, -1.0f, 0, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 6) // right
                                {
                                    enemycomponent.move = 6;
                                    std::cout << "ENEMY TOP SIDE MOVE RIGHT" << std::endl;
                                    enemycomponent.dirX = 1.0f;
                                    enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                                }
                                else if (!IsObstacleInFront(enemyX, enemyY, 1.0f, 0, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 4) // left
                                {
                                    enemycomponent.move = 4;
                                    std::cout << "ENEMY TOP SIDE MOVE LEFT" << std::endl;
                                    enemycomponent.dirX = -1.0f;
                                    enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                                }
                                else
                                {
                                    std::cout << "ENEMY TOP SIDE MOVE TOP" << std::endl;
                                    enemycomponent.move = 8;
                                    enemyphysic.force.y = enemycomponent.speed;
                                }
                            }
                            else
                            {
                                if (!IsObstacleInFront(enemyX, enemyY, 1.0f, 0, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 4) // left
                                {
                                    enemycomponent.move = 4;
                                    // std::cout << "ENEMY TOP SIDE MOVE LEFT" << std::endl;
                                    enemycomponent.dirX = -1.0f;
                                    enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                                }
                                else if (!IsObstacleInFront(enemyX, enemyY, -1.0f, 0, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 6) // right
                                {
                                    enemycomponent.move = 6;
                                    // std::cout << "ENEMY TOP SIDE MOVE RIGHT" << std::endl;
                                    enemycomponent.dirX = 1.0f;
                                    enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                                }
                                else
                                {
                                    // std::cout << "ENEMY TOP SIDE MOVE TOP" << std::endl;
                                    enemycomponent.move = 8;
                                    enemyphysic.force.y = enemycomponent.speed;
                                }
                            }
                        }
                        else if (enemytransform.position.y < collidedObj->GetComponent<Transform>().position.y - collidedObj->GetComponent<Transform>().scale.x * 0.5f) // btm
                        {
                            // std::cout << "ENEMY BTM SIDE" << std::endl;
                            enemyphysic.force.y = 0; // Stop vertical movement

                            // check where player
                            if (playerObj->GetComponent<Transform>().position.x < enemytransform.position.x) // player on left
                            {
                                if (!IsObstacleInFront(enemyX, enemyY, 1.0f, 0, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 4) // left
                                {
                                    enemycomponent.move = 4;
                                    // std::cout << "ENEMY Bottom SIDE MOVE LEFT" << std::endl;
                                    enemycomponent.dirX = -1.0f;
                                    enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                                }
                                else if (!IsObstacleInFront(enemyX, enemyY, -1.0f, 0, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 6) // right
                                {
                                    enemycomponent.move = 6;
                                    // std::cout << "ENEMY Bottom SIDE MOVE RIGHT" << std::endl;
                                    enemycomponent.dirX = 1.0f;
                                    enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                                }
                                else
                                {
                                    // std::cout << "ENEMY Bottom SIDE MOVE BTM" << std::endl;
                                    enemycomponent.move = 2;
                                    enemyphysic.force.y = -enemycomponent.speed;
                                }
                            }
                            else
                            {
                                if (!IsObstacleInFront(enemyX, enemyY, -1.0f, 0, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 6) // right
                                {
                                    enemycomponent.move = 6;
                                    // std::cout << "ENEMY Bottom SIDE MOVE RIGHT" << std::endl;
                                    enemycomponent.dirX = 1.0f;
                                    enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                                }
                                else if (!IsObstacleInFront(enemyX, enemyY, 1.0f, 0, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 4) // left
                                {
                                    enemycomponent.move = 4;
                                    // std::cout << "ENEMY Bottom SIDE MOVE LEFT" << std::endl;
                                    enemycomponent.dirX = -1.0f;
                                    enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                                }
                                else
                                {
                                    // std::cout << "ENEMY Bottom SIDE MOVE BTM" << std::endl;
                                    enemycomponent.move = 2;
                                    enemyphysic.force.y = -enemycomponent.speed;
                                }
                            }
                        }
                        else
                        {
                            enemyphysic.force.x = 0;                                                                                                                   // Stop horizontal movement
                            if (enemytransform.position.x > collidedObj->GetComponent<Transform>().position.x + collidedObj->GetComponent<Transform>().scale.x * 0.5f) // right
                            {
                                std::cout << "ENEMY RIGHT SIDE" << std::endl;
                                // if right side try move up / down
                                //  5 = center (not moving) , 8 is up , 4 is left , 6 is right , 2 is down
                                if (!IsObstacleInFront(enemyX, enemyY, 0, 1.0f, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 8) // Move up if possible
                                {
                                    // std::cout << "ENEMY RIGHT SIDE MOVE TOP" << std::endl;
                                    enemycomponent.move = 8;
                                    enemyphysic.force.y = enemycomponent.speed;
                                }
                                else if (!IsObstacleInFront(enemyX, enemyY, 0, -1.0f, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 2) // Move down if possible
                                {
                                    enemycomponent.move = 2;
                                    // std::cout << "ENEMY RIGHT SIDE MOVE BTM" << std::endl;
                                    enemyphysic.force.y = -enemycomponent.speed;
                                }
                                else
                                {
                                    enemycomponent.move = 6;
                                    // std::cout << "ENEMY RIGHT SIDE MOVE RIGHT" << std::endl;
                                    enemycomponent.dirX = 1.0f;
                                    enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                                }
                            }
                            else if (enemytransform.position.x < collidedObj->GetComponent<Transform>().position.y - collidedObj->GetComponent<Transform>().scale.x * 0.5f) // left
                            {
                                std::cout << "ENEMY LEFT SIDE" << std::endl;
                                if (!IsObstacleInFront(enemyX, enemyY, 0, 1.0f, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 8) // Move up if possible
                                {
                                    // std::cout << "ENEMY LEFT SIDE MOVE TOP" << std::endl;
                                    enemycomponent.move = 8;
                                    enemyphysic.force.y = enemycomponent.speed;
                                }
                                else if (!IsObstacleInFront(enemyX, enemyY, 0, -1.0f, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 2) // Move down if possible
                                {
                                    enemycomponent.move = 2;
                                    // std::cout << "ENEMY LEFT SIDE MOVE BTM" << std::endl;
                                    enemyphysic.force.y = -enemycomponent.speed;
                                }
                                else
                                {
                                    enemycomponent.move = 4;
                                    // std::cout << "ENEMY LEFT SIDE MOVE LEFT" << std::endl;
                                    enemycomponent.dirX = -1.0f;
                                    enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                                }
                            }
                        }

                        //// Collision handling
                        // if (absDeltaX > absDeltaY)  // More horizontal collision
                        //{
                        //     enemyphysic.force.y = 0;  // Stop horizontal movement
                        //     if (deltaY > 0)  // Enemy is on top of the object
                        //     {
                        //         std::cout << "ENEMY TOP SIDE" << std::endl;
                        //         //check where player
                        //         if (playerObj->GetComponent<Transform>().position.x > enemytransform.position.x) // player on right
                        //         {
                        //             if (!IsObstacleInFront(enemyX, enemyY, -1.0f, 0, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 6) //right
                        //             {
                        //                 enemycomponent.move = 6;
                        //                 std::cout << "ENEMY TOP SIDE MOVE RIGHT" << std::endl;
                        //                 enemycomponent.dirX = 1.0f;
                        //                 enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                        //             }
                        //             else if (!IsObstacleInFront(enemyX, enemyY, 1.0f, 0, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 4) //left
                        //             {
                        //                 enemycomponent.move = 4;
                        //                 std::cout << "ENEMY TOP SIDE MOVE LEFT" << std::endl;
                        //                 enemycomponent.dirX = -1.0f;
                        //                 enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                        //             }
                        //             else
                        //             {
                        //                 std::cout << "ENEMY TOP SIDE MOVE TOP" << std::endl;
                        //                 enemycomponent.move = 8;
                        //                 enemyphysic.force.y = enemycomponent.speed;
                        //             }
                        //         }
                        //         else
                        //         {
                        //             if (!IsObstacleInFront(enemyX, enemyY, 1.0f, 0, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 4) //left
                        //             {
                        //                 enemycomponent.move = 4;
                        //                 std::cout << "ENEMY TOP SIDE MOVE LEFT" << std::endl;
                        //                 enemycomponent.dirX = -1.0f;
                        //                 enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                        //             }
                        //             else if (!IsObstacleInFront(enemyX, enemyY, -1.0f, 0, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 6) //right
                        //             {
                        //                 enemycomponent.move = 6;
                        //                 std::cout << "ENEMY TOP SIDE MOVE RIGHT" << std::endl;
                        //                 enemycomponent.dirX = 1.0f;
                        //                 enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                        //             }
                        //             else
                        //             {
                        //                 std::cout << "ENEMY TOP SIDE MOVE TOP" << std::endl;
                        //                 enemycomponent.move = 8;
                        //                 enemyphysic.force.y = enemycomponent.speed;
                        //             }
                        //         }

                        //    }
                        //    else if (deltaY < 0)// Enemy is on bottom of the object
                        //    {
                        //        std::cout << "ENEMY Bottom SIDE" << std::endl;
                        //        //check where player
                        //        if (playerObj->GetComponent<Transform>().position.x < enemytransform.position.x) // player on left
                        //        {
                        //            if (!IsObstacleInFront(enemyX, enemyY, 1.0f, 0, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 4) //left
                        //            {
                        //                enemycomponent.move = 4;
                        //                std::cout << "ENEMY Bottom SIDE MOVE LEFT" << std::endl;
                        //                enemycomponent.dirX = -1.0f;
                        //                enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                        //            }
                        //            else if (!IsObstacleInFront(enemyX, enemyY, -1.0f, 0, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 6) //right
                        //            {
                        //                enemycomponent.move = 6;
                        //                std::cout << "ENEMY Bottom SIDE MOVE RIGHT" << std::endl;
                        //                enemycomponent.dirX = 1.0f;
                        //                enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                        //            }
                        //            else
                        //            {
                        //                std::cout << "ENEMY Bottom SIDE MOVE BTM" << std::endl;
                        //                enemycomponent.move = 2;
                        //                enemyphysic.force.y = -enemycomponent.speed;
                        //            }
                        //        }
                        //        else
                        //        {
                        //            if (!IsObstacleInFront(enemyX, enemyY, -1.0f, 0, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 6) //right
                        //            {
                        //                enemycomponent.move = 6;
                        //                std::cout << "ENEMY Bottom SIDE MOVE RIGHT" << std::endl;
                        //                enemycomponent.dirX = 1.0f;
                        //                enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                        //            }
                        //            else if (!IsObstacleInFront(enemyX, enemyY, 1.0f, 0, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 4) //left
                        //            {
                        //                enemycomponent.move = 4;
                        //                std::cout << "ENEMY Bottom SIDE MOVE LEFT" << std::endl;
                        //                enemycomponent.dirX = -1.0f;
                        //                enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                        //            }
                        //            else
                        //            {
                        //                std::cout << "ENEMY Bottom SIDE MOVE BTM" << std::endl;
                        //                enemycomponent.move = 2;
                        //                enemyphysic.force.y = -enemycomponent.speed;
                        //            }
                        //        }
                        //    }

                        //}
                        // else // vertical collision
                        //{
                        //    enemyphysic.force.x = 0;  // Stop vertical movement

                        //     if (enemytransform.position.y > collidedObj->GetComponent<Transform>().position.y + 0.25f * collidedObj->GetComponent<Transform>().scale.y ||
                        //        enemytransform.position.y < collidedObj->GetComponent<Transform>().position.y - 0.5f * collidedObj->GetComponent<Transform>().scale.y)
                        //     {
                        //         std::cout << "ENEMY OUTSIDE" << std::endl;
                        //        enemycomponent.state = Enemy::MOVE;
                        //        enemycomponent.isCollide = false;
                        //        enemycomponent.timeSinceTargetReached = 0.f;
                        //        enemycomponent.move = 5;
                        //        break;
                        //     }

                        //    if (deltaX < 0)  // Enemy is on the right side of the object
                        //    {
                        //        std::cout << "ENEMY RIGHT SIDE" << std::endl;
                        //        //if right side try move up / down
                        //        // 5 = center (not moving) , 8 is up , 4 is left , 6 is right , 2 is down
                        //        if (!IsObstacleInFront(enemyX, enemyY, 0, 1.0f, 100.0f)&& enemycomponent.move == 5 || enemycomponent.move == 8) // Move up if possible
                        //        {
                        //            std::cout << "ENEMY RIGHT SIDE MOVE TOP" << std::endl;
                        //            enemycomponent.move = 8;
                        //            enemyphysic.force.y = enemycomponent.speed;
                        //        }
                        //        else if (!IsObstacleInFront(enemyX, enemyY, 0, -1.0f, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 2) // Move down if possible
                        //        {
                        //            enemycomponent.move = 2;
                        //            std::cout << "ENEMY RIGHT SIDE MOVE BTM" << std::endl;
                        //            enemyphysic.force.y = -enemycomponent.speed;
                        //        }
                        //        else
                        //        {
                        //            enemycomponent.move = 6;
                        //            std::cout << "ENEMY RIGHT SIDE MOVE RIGHT" << std::endl;
                        //            enemycomponent.dirX = 1.0f;
                        //            enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                        //        }
                        //        //if (!IsObstacleInFront(enemyX, enemyY, 1.0f, 0, 100.0f)) //left
                        //        //{
                        //        //    std::cout << "LEFT" << std::endl;
                        //        //}
                        //        //if (!IsObstacleInFront(enemyX, enemyY, -1.0f, 0, 100.0f)) //right
                        //        //{
                        //        //    std::cout << "RIGHT" << std::endl;
                        //        //}
                        //        //if (!IsObstacleInFront(enemyX, enemyY, 0, 1.f, 100.0f))//TOP
                        //        //{
                        //        //    std::cout << "TOP" << std::endl;
                        //        //}
                        //        //if (!IsObstacleInFront(enemyX, enemyY, 0, -1.0f, 100.0f))//BTM
                        //        //{
                        //        //    std::cout << "BTM" << std::endl;
                        //        //}
                        //    }
                        //    else if (deltaX > 0)// Enemy is on the left side of the object
                        //    {
                        //        std::cout << "ENEMY LEFT SIDE" << std::endl;
                        //        if (!IsObstacleInFront(enemyX, enemyY, 0, 1.0f, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 8) // Move up if possible
                        //        {
                        //            std::cout << "ENEMY LEFT SIDE MOVE TOP" << std::endl;
                        //            enemycomponent.move = 8;
                        //            enemyphysic.force.y = enemycomponent.speed;
                        //        }
                        //        else if (!IsObstacleInFront(enemyX, enemyY, 0, -1.0f, 100.0f) && enemycomponent.move == 5 || enemycomponent.move == 2) // Move down if possible
                        //        {
                        //            enemycomponent.move = 2;
                        //            std::cout << "ENEMY LEFT SIDE MOVE BTM" << std::endl;
                        //            enemyphysic.force.y = -enemycomponent.speed;
                        //        }
                        //        else
                        //        {
                        //            enemycomponent.move = 4;
                        //            std::cout << "ENEMY LEFT SIDE MOVE LEFT" << std::endl;
                        //            enemycomponent.dirX = -1.0f;
                        //            enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                        //        }
                        //    }

                        //}

                        //// Collision handling
                        // if (absDeltaX > absDeltaY)  // More horizontal collision
                        //{
                        //     enemyphysic.force.x = 0;  // Stop horizontal movement

                        //    if (deltaY > 0)  // Enemy is on top of the object
                        //    {
                        //        if (!IsObstacleInFront(enemyX, enemyY, 1.0f, 0, 1000.0f)) // Move right if possible
                        //        {
                        //            enemycomponent.dirX = 1.0f;
                        //            enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                        //        }
                        //        else if (!IsObstacleInFront(enemyX, enemyY, -1.0f, 0, 1000.0f)) // Move left if possible
                        //        {
                        //            enemycomponent.dirX = -1.0f;
                        //            enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                        //        }
                        //        else // Both left and right blocked, force up movement
                        //        {
                        //            enemyphysic.force.y = enemycomponent.speed;  // Move up
                        //        }
                        //    }
                        //    else if (deltaY < 0)  // Enemy is at the bottom of the object
                        //    {
                        //        if (!IsObstacleInFront(enemyX, enemyY, 1.0f, 0, 1000.0f)) // Move right if possible
                        //        {
                        //            enemycomponent.dirX = 1.0f;
                        //            enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                        //        }
                        //        else if (!IsObstacleInFront(enemyX, enemyY, -1.0f, 0, 1000.0f)) // Move left if possible
                        //        {
                        //            enemycomponent.dirX = -1.0f;
                        //            enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                        //        }
                        //        else // Both left and right blocked, force down movement
                        //        {
                        //            enemyphysic.force.y = -enemycomponent.speed;  // Move down
                        //        }
                        //    }
                        //}
                        // else  // More vertical collision
                        //{
                        //    enemyphysic.force.y = 0;  // Stop vertical movement

                        //    if (enemytransform.position.y < collidedObj->GetComponent<Transform>().position.y - 0.5f * collidedObj->GetComponent<Transform>().scale.y ||
                        //        enemytransform.position.y > collidedObj->GetComponent<Transform>().position.y + 0.5f * collidedObj->GetComponent<Transform>().scale.y)
                        //    {
                        //        enemycomponent.state = Enemy::MOVE;
                        //        enemycomponent.isCollide = false;
                        //        enemycomponent.timeSinceTargetReached = 0.f;
                        //        break;
                        //    }

                        //    if (deltaX < 0)  // Enemy is on the left side of the object
                        //    {
                        //        if (!IsObstacleInFront(enemyX, enemyY, 0, 1.0f, 1000.0f)) // Move up if possible
                        //        {
                        //            enemyphysic.force.y = enemycomponent.speed;
                        //        }
                        //        else if (!IsObstacleInFront(enemyX, enemyY, 0, -1.0f, 1000.0f)) // Move down if possible
                        //        {
                        //            enemyphysic.force.y = -enemycomponent.speed;
                        //        }
                        //        else // Both up and down blocked, force movement to the left
                        //        {
                        //            enemycomponent.dirX = -1.0f;
                        //            enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                        //        }
                        //    }
                        //    else if (deltaX > 0)  // Enemy is on the right side of the object
                        //    {
                        //        if (!IsObstacleInFront(enemyX, enemyY, 0, 1.0f, 1000.0f)) // Move up if possible
                        //        {
                        //            enemyphysic.force.y = enemycomponent.speed;
                        //        }
                        //        else if (!IsObstacleInFront(enemyX, enemyY, 0, -1.0f, 1000.0f)) // Move down if possible
                        //        {
                        //            enemyphysic.force.y = -enemycomponent.speed;
                        //        }
                        //        else // Both up and down blocked, force movement to the right
                        //        {
                        //            enemycomponent.dirX = 1.0f;
                        //            enemyphysic.force.x = enemycomponent.dirX * enemycomponent.speed;
                        //        }
                        //    }
                        //}
                    }
                    ++it;
                    continue;
                }

                // ENEMY STATE MACHINE
                switch (enemycomponent.state)
                {
                    // MOVE
                case Enemy::MOVE:

                    // Compute the direction vector to the target (player)
                    enemycomponent.dirX = playerObj->GetComponent<Transform>().position.x - enemytransform.position.x;
                    enemycomponent.dirY = playerObj->GetComponent<Transform>().position.y - enemytransform.position.y;

                    if (enemycomponent.IsPlayerInAttackRange(playerObj->GetComponent<Transform>(), enemytransform) == false)
                    {
                        if (enemycomponent.iswaiting)
                            break;

                        if (object->GetComponent<Animation>().currentClip != "Walk")
                        {
                            object->GetComponent<Animation>().SetAnimation("Walk");
                        }
                        if (enemycomponent.IsPlayerInRange(playerObj->GetComponent<Transform>(), enemytransform) == false)
                        {
                            // small movement?

                            // BIG movement
                            //  Calculate the magnitude of the direction vector (distance to player)
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
                            if (distY * distY > 400.f)
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
                            enemycomponent.waitTime = .5f;
                            enemycomponent.iswaiting = true;
                        }
                    }

                    break;

                case Enemy::STANDBY:
                    if (object->GetComponent<Animation>().currentClip != "Idle")
                    {
                        object->GetComponent<Animation>().SetAnimation("Idle");
                    }

                    if (!enemycomponent.isWithPlayer)
                    {
                        if (enemycomponent.IsPlayerInAttackRange(playerObj->GetComponent<Transform>(), enemytransform) == true)
                        {
                            enemycomponent.isWithPlayer = true;
                            if (numEnemyTarget < 2)
                            {
                                ++numEnemyTarget;
                                enemycomponent.state = Enemy::ATTACK;
                                break;
                            }
                        }
                        else
                        {
                            if (!enemycomponent.iswaiting)
                            {
                                enemycomponent.iswaiting = true;
                                enemycomponent.waitTime = 0.5f;
                                break;
                            }
                        }
                    }
                    else
                    {
                        if (enemycomponent.IsPlayerInAttackRange(playerObj->GetComponent<Transform>(), enemytransform) == false)
                        {
                            if (enemycomponent.isWithPlayer == true)
                            {
                                enemycomponent.isWithPlayer = false;
                                --numEnemyTarget;
                                break;
                            }
                        }
                        else
                        {
                            if (numEnemyTarget < 2)
                            {
                                enemycomponent.state = Enemy::ATTACK;
                                break;
                            }
                        }
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

                    // Charge attack for fish
                    if (enemycomponent.type == enemycomponent.FISH)
                    {

                        if (object->GetComponent<Animation>().currentClip == "Attack" && object->GetComponent<Animation>().current_frame == 18)
                        {
                            // SFX
                            auto &audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                            std::vector<int> fishAttackSounds;

                            // Add all available fish attack sounds to the array
                            int fishAttack1 = audioM.GetSFXindex("FishAttack1");
                            if (fishAttack1 != -1)
                                fishAttackSounds.push_back(fishAttack1);

                            int fishAttack2 = audioM.GetSFXindex("FishAttack2");
                            if (fishAttack2 != -1)
                                fishAttackSounds.push_back(fishAttack2);

                            int fishAttack3 = audioM.GetSFXindex("FishAttack3");
                            if (fishAttack3 != -1)
                                fishAttackSounds.push_back(fishAttack3);

                            // Add the original sound as fallback
                            int fishAttackOriginal = audioM.GetSFXindex("FishAttack1");
                            if (fishAttackOriginal != -1)
                                fishAttackSounds.push_back(fishAttackOriginal);

                            // If we have fish attack sounds available, play a random one
                            if (!fishAttackSounds.empty())
                            {
                                // Generate a random index into our array of sounds
                                int randomIndex = rand() % fishAttackSounds.size();
                                int selectedSoundIndex = fishAttackSounds[randomIndex];

                                // Check if the selected sound is already playing
                                if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(selectedSoundIndex))
                                {
                                    audioM.PlaySFX(selectedSoundIndex);

                                    // check attack
                                    if (enemycomponent.IsPlayerInAttackRange(playerObj->GetComponent<Transform>(), enemytransform))
                                    {
                                        // attack
                                        if (playerObj != nullptr)
                                        {
                                            enemycomponent.AttackPlayer(playerObj->GetComponent<Player>().currentHealth);
                                            ECS::GetInstance().GetSystem<PlayerManager>()->OnCollisionEnter(playerObj->GetInstanceID());
                                        }
                                    }
                                }
                            }
                        }
                        if (object->GetComponent<Animation>().currentClip == "Attack" && object->GetComponent<Animation>().current_frame == 7)
                        {
                            // SFX for fish in air
                            auto &audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();

                            // Create an array to hold our fish jump/air sound indices
                            std::vector<int> fishAirSounds;

                            // Add all available fish air sounds to the array
                            int fishAir1 = audioM.GetSFXindex("FishAir1");
                            if (fishAir1 != -1)
                                fishAirSounds.push_back(fishAir1);

                            int fishAir2 = audioM.GetSFXindex("FishAir2");
                            if (fishAir2 != -1)
                                fishAirSounds.push_back(fishAir2);

                            // Add the original sound as fallback if you have one
                            int fishAirOriginal = audioM.GetSFXindex("FishAir");
                            if (fishAirOriginal != -1)
                                fishAirSounds.push_back(fishAirOriginal);

                            // If we have fish air sounds available, play a random one
                            if (!fishAirSounds.empty())
                            {
                                // Generate a random index into our array of sounds
                                int randomIndex = rand() % fishAirSounds.size();
                                int selectedSoundIndex = fishAirSounds[randomIndex];

                                // Check if the selected sound is already playing
                                if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(selectedSoundIndex))
                                {
                                    audioM.PlaySFX(selectedSoundIndex);
                                }
                            }
                        }

                        if (object->GetComponent<Animation>().currentClip == "Attack" && object->GetComponent<Animation>().current_frame == 22)
                        {
                            if (!enemycomponent.IsPlayerInAttackRange(playerObj->GetComponent<Transform>(), enemytransform))
                            {
                                enemycomponent.iswaiting = true;
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
                            if (!enemycomponent.IsPlayerInAttackRange(playerObj->GetComponent<Transform>(), enemytransform))
                            {
                                enemycomponent.iswaiting = true;
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
                            // Play SFX
                            auto &audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                            if (audioM.GetSFXindex("WormAttack") != -1)
                            {
                                if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("WormAttack")))
                                {
                                    audioM.PlaySFX(audioM.GetSFXindex("WormAttack"));
                                }
                            }

                            // spawn bullet
                            static int number = 0;
                            GameObject *cloneObject = GameObjectManager::GetInstance().GetGOByTag("EnemyProjectile1");

                            if (cloneObject != nullptr)
                            {
                                std::string name = "bullet" + std::to_string(number++);
                                GameObject &newObject = GameObjectManager::GetInstance().CloneObject(*cloneObject, name, "EnemyProjectile");

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
                // END OF FOR LOOP
            }
        }
    }

    bool EnemyManager::HasClearLineOfSight(GameObject *enemy, const Transform &player)
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

            GameObject *object = GameObjectManager::GetInstance().GetGO(objectID);

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
        if (length == 0)
        {
            std::cout << "Invalid direction vector (0,0)!" << std::endl;
            return false;
        }
        // if (length == 0) return false; // Avoid division by zero

        float normDirX = dirX / length;
        float normDirY = dirY / length;

        // Compute the endpoint of the check line
        float endX = startX + normDirX * range;
        float endY = startY + normDirY * range;

        for (EntityID objectID : ECS::GetInstance().GetSystem<DungeonManager>()->rooms[ECS::GetInstance().GetSystem<DungeonManager>()->current_room_id].entities)
        {
            GameObject *object = GameObjectManager::GetInstance().GetGO(objectID);
            if (!object)
                continue;

            if (object->GetTag() == "Environment" || object->GetTag() == "Boundary")
            {
                Transform &objTransform = object->GetComponent<Transform>();
                float objX = objTransform.position.x;
                float objY = objTransform.position.y;
                float objWidth = objTransform.scale.x;  // Half-width for AABB check
                float objHeight = objTransform.scale.y; // Half-height for AABB check
                if (Enemy::LineIntersectsRect(endX, endY, normDirX, normDirY, length, objX, objY, objWidth, objHeight))
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
        auto &gameObjectManager = GameObjectManager::GetInstance();
        auto enemy = gameObjectManager.GetGO(enemyID);
        auto obj2 = gameObjectManager.GetGO(objID);

        auto &enemyComponent = enemy->GetComponent<Enemy>();

        if (obj2->HasComponent<Enemy>()) // NOT IN USED
        {
            auto &enemyComponent2 = obj2->GetComponent<Enemy>();
            enemyComponent2.isCollide = false;
            enemyComponent.isCollide = false;
        }
        else
        {
            // set collide to true then now the obj is the obj save the pathfinding obj as prev
            enemyComponent.isCollide = true;
            enemyComponent.timeSinceTargetReached = 1.f;
            enemyComponent.collideObj = static_cast<int>(objID);
            // enemyComponent.prevObject = static_cast<int>(enemyComponent.nearestObj);
            // enemyComponent.nearestObj = static_cast<int>(objID);
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
    bool EnemyManager::IsClearPathToPosition(GameObject *enemy, float newX, float newY)
    {
        // Check if moving the enemy to the new position (newX, newY) is clear of obstacles

        for (EntityID objectID : environmentObjects)
        {
            GameObject *object = GameObjectManager::GetInstance().GetGO(objectID);
            Transform &objectTransform = object->GetComponent<Transform>();

            // Calculate if the potential new position collides with any other environment object
            float dx = newX - objectTransform.position.x;
            float dy = newY - objectTransform.position.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            if (newX < enemy->GetComponent<Transform>().scale.x || newX > 1600 || newY < 0 || newY > 900)
            {
                return false;
            }

            // If the new position is too close to the environment object, it's not clear
            if (distance < 1.0f) // Adjust this threshold as needed
            {
                return false;
            }
        }

        return true; // The path is clear to move
    }

    /*!***********************************************************************
    \brief
        Finds the nearest object to a given enemy.
    \param enemy: Pointer to the GameObject representing the enemy.
    \return
        The ID of the nearest object to the enemy.
    *************************************************************************/
    int EnemyManager::FindNearestObject(GameObject *enemy) const
    {
        if (environmentObjects.empty())
        {
            return -1; // No objects available
        }

        // Collect all eligible objects excluding the previous object
        std::vector<GameObject *> eligibleObjects;
        for (EntityID objectID : environmentObjects)
        {
            GameObject *object = GameObjectManager::GetInstance().GetGO(objectID);
            if (object->GetInstanceID() != enemy->GetComponent<Enemy>().prevObject)
            {
                eligibleObjects.push_back(object);
            }
        }

        // If there are no eligible objects, return -1
        if (eligibleObjects.empty())
        {
            return -1;
        }

        // Select a random object from the eligible list
        int randomIndex = std::rand() % eligibleObjects.size();
        GameObject *selectedObject = eligibleObjects[randomIndex];

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
    bool EnemyManager::IsEnemyAwayFromObject(GameObject *enemy, GameObject *targetObject, float minDistanceThreshold) const
    {
        if (!enemy || !targetObject)
        {
            return true; // Consider "away" if either is null
        }

        // Get positions
        const auto &enemyPos = enemy->GetComponent<Transform>().position;
        const auto &objectPos = targetObject->GetComponent<Transform>().position;

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
                GameObject *object = GameObjectManager::GetInstance().GetGO(*it);
                if (object->GetComponent<Enemy>().state == Enemy::DEAD)
                {
                    GameObjectManager::GetInstance().DestroyObject(object->GetInstanceID());
                }
            }
            enemyObjects.clear();
        }
    }

}
