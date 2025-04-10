/* Start Header ************************************************************************/
/*!
\file       Components.h
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu (50%)
\co-authors Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu (25%)
\co-authors Tan Si Han, t.sihan, 2301264, t.sihan\@digipen.edu (10%)
\date       Apr 1, 2025
\brief      Here is where we store all the different components that are needed to be added or removed (i.e Transform, Sprite, etc).

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include <glad/glad.h> // for GLuint
#include "PreCompile.h"
#include "../Math/Matrix4x4.h" // for Mtx44 struct
#include "../Math/Vector2D.h"  // for Vec2 struct
#include "../Audio/Audio.h"	   // for Audio class

namespace Ukemochi
{
	/*!***********************************************************************
	\brief
	 Transform component structure.
	*************************************************************************/
	struct Transform
	{
		Mtx44 transform_matrix;
		Vec3 position;
		float rotation = 0.f;
		Vec2 scale;
	};

	/*!***********************************************************************
	\brief
	 Rigidbody2D component structure.
	*************************************************************************/
	struct Rigidbody2D
	{
		// Linear Properties
		Vec3 position{};
		Vec2 velocity{};
		Vec2 acceleration{};
		Vec2 force{};
		float mass{1.f};				// Minimum mass of 1
		float inverse_mass{1.f / mass}; // inverse mass = 1/mass
		float linear_drag{0.9f};		// Adjust to control the friction from [0, 1]

		// Rotational Properties
		float angle{};
		float angular_velocity{};
		float angular_acceleration{};
		float torque{};
		float inertia_mass{1.f};					// Minimum inertia mass of 1
		float inv_inertia_mass{1.f / inertia_mass}; // inverse inertia mass = 1/inertia mass
		float angular_drag{0.9f};					// Adjust to control the friction from [0, 1]

		bool use_gravity{false};  // If true, apply gravity
		bool is_kinematic{false}; // If true, don't apply physics
	};

	/*!***********************************************************************
	\brief
	 BoxCollider2D component structure.
	*************************************************************************/
	struct BoxCollider2D
	{
		// Min,max for AABB
		Vec2 min{};
		Vec2 max{};

		int collision_flag{};	// Track the collision flags
		bool is_trigger{false}; // If true, act as a trigger
	};

	/*!***********************************************************************
	\brief
	 CircleCollider2D component structure.
	*************************************************************************/
	struct CircleCollider2D
	{
		Vec2 m_center{};
		float m_radius{};
	};

	/*!***********************************************************************
	\brief
	 ConvexCollider2D component structure.
	*************************************************************************/
	struct ConvexCollider2D
	{
		std::vector<Vec2> vertices;
	};

	/*!***********************************************************************
	\brief
	 Sprite shape enumeration.
	*************************************************************************/
	enum class SPRITE_SHAPE
	{
		BOX = 0,
		CIRCLE = 1
	}; // Enum for the shape of the sprite, THIS FOLLOWS THE GRAPHICS RENDERER

	/*!***********************************************************************
	\brief
	 AnimationClip structure.
	*************************************************************************/
	struct AnimationClip
	{
		std::string spriteName{};		// Name of the spritesheet inside the atlas texture
		std::string keyPath{};
		std::string name{};				 // Name of the animation clip
		Vec2 pivot = Vec2{32.0f, 32.0f}; // Pivot point of the sprite
		// GLuint textureID = 0;		// The texture ID, to be set during runtime
		int pixelsPerUnit = 100;        // Pixels per unit
		int total_frames = 1;	  // Total frames in the animation
		int pixel_width = 64;	  // Width of each frame in pixels
		int pixel_height = 64;	  // Height of each frame in pixels
		int total_width = 64;	  // Width of the sprite sheet
		int total_height = 64;	  // Height of the sprite sheet
		float frame_time = 0.05f; // Duration of each frame
		bool looping = true;	  // Should the animation loop?

		// AnimationClip();
		// AnimationClip(std::string name, GLuint textureID, int total_frames, int pixel_width, int pixel_height, int total_width, int total_height, float frameTime, bool looping)
		// 	: name(std::move(name)), textureID(textureID), total_frames(total_frames), pixel_width(pixel_width), pixel_height(pixel_height), total_width(total_width), total_height(total_height), frame_time(frameTime), looping(looping) {}
	};

	/*!***********************************************************************
	\brief
	 Animation component structure.
	 Manages frame-based animations for entities.
	*************************************************************************/
	struct Animation
	{
		std::unordered_map<std::string, AnimationClip> clips; // Animation clips
		std::string currentClip{};							  // Name of the active animation.
		std::string defaultClip{};							  // Name of the default animation.
		int current_frame = 0;								  // Current frame index
		int stop_frame = 0;
		float time_since_last_frame = 0.0f;					  // Time since the last frame
		float original_frame_time = 0.05f;					  // Original frame time
		bool is_playing = true;								  // Is the animation playing?
		bool doNotInterrupt = false;						  // Do not interrupt the current animation
		bool isReverse = false;

		bool isAttacking = false;
		bool attackAnimationFinished = false;

		bool SetAnimation(const std::string &name)
		{
			if (clips.find(name) != clips.end() && name != currentClip && !doNotInterrupt && !isAttacking)
			{
				currentClip = name;
				current_frame = 0;
				stop_frame = 0;
				time_since_last_frame = 0.0f;

				return true;
			}
			return false;
		}

		bool SetAnimation(const std::string &name, const int initialFrame)
		{
			doNotInterrupt = false;
			isAttacking = false;
			if (clips.find(name) != clips.end() && name != currentClip && !doNotInterrupt && !isAttacking)
			{
				currentClip = name;
				current_frame = initialFrame;
				stop_frame = 0;
				time_since_last_frame = 0.0f;

				return true;
			}
			return false;
		}

		bool SetAnimationFromTo(const std::string &name, const int initialFrame, const int untilFrame)
		{
			doNotInterrupt = false;
			isAttacking = false;
			if (clips.find(name) != clips.end() && name != currentClip && !doNotInterrupt && !isAttacking)
			{
				currentClip = name;
				current_frame = initialFrame;
				stop_frame = untilFrame;
				time_since_last_frame = 0.0f;

				doNotInterrupt = true;
				isAttacking = true;
				return true;
			}

			return false;
		}

		bool SetAnimationImmediately(const std::string &name)
		{
			doNotInterrupt = false;
			isAttacking = false;
			return SetAnimation(name);
		}

		bool SetAnimationImmediately(const std::string &name, const int initialFrame)
		{
			doNotInterrupt = false;
			isAttacking = false;
			return SetAnimation(name,initialFrame);
		}

		bool SetAnimationUninterrupted(const std::string &name)
		{
			if (clips.find(name) != clips.end() && name != currentClip && !doNotInterrupt)
			{
				currentClip = name;
				current_frame = 0;
				time_since_last_frame = 0.0f;

				doNotInterrupt = true;

				return true;
			}
			return false;
		}

		bool RestartAnimation()
		{
			if (clips.find(currentClip) == clips.end())
				return false;

			current_frame = 0;
			time_since_last_frame = 0.0f;

			return true;
		}

		void update(float dt)
		{
			if (clips.find(currentClip) == clips.end()) // Don't update if the clip doesn't exist
				return;

			if (!is_playing) // Don't update if the animation is set not to play
				return;

			AnimationClip &clip = clips[currentClip];
			time_since_last_frame += dt;

			if (isAttacking)
			{
				attackAnimationFinished = false;
			}
			
			// Advance new frame
			if (time_since_last_frame >= clip.frame_time)
			{
				if (!isReverse)
				{
					current_frame++;
				
					if (current_frame >= clip.total_frames || (current_frame >= stop_frame && stop_frame != 0))
					{
						current_frame = clip.looping ? 0 : clip.total_frames - 1;
						doNotInterrupt = false;
						isAttacking = false;
					}
				}
				else
				{
					current_frame--;

					if (current_frame < 0 || current_frame <= stop_frame)
                    {
                        current_frame = clip.looping ? clip.total_frames - 1 : 0;
                        doNotInterrupt = false;
                        isAttacking = false;
						isReverse = false; // reset animation to forward
                    }
				}

				time_since_last_frame = 0.0f; // Reset time
			}
			// Renderer system handles the UV coordinates for us
		}

		void setCurrentClipDuration(float newDuration)
		{
			if (clips.find(currentClip) == clips.end())
				return;

			original_frame_time = clips[currentClip].frame_time;
			clips[currentClip].frame_time = newDuration;
		}

		void resetCurrentClipDuration()
		{
			if (clips.find(currentClip) == clips.end())
				return;

			clips[currentClip].frame_time = original_frame_time;
		}

		int GetCurrentFrame() const
		{
			return current_frame;
		}

	};

	/*!***********************************************************************
	\brief
	 Render layer enumeration.
	*************************************************************************/
	enum RenderLayer // The type of render layers
	{
		BACKGROUND,			  // to render background objects
		SUB_DYNAMIC_BACK,	  // to render skills, projectile and shadow objects behind dynamic and static objects
		DYNAMIC_BACK,		  // to render player and enemy objects behind static objects
		STATIC,				  // to render static objects
		SUB_DYNAMIC_FRONT,	  // to render skills, projectile and shadow objects behind dynamic objects and infront of static objects
		DYNAMIC_FRONT,		  // to render enemy objects infront of static objects
		PLAYER_DYNAMIC_FRONT, // to render player infront of static objects
		FOREGROUND			  // to render foreground objects
	};

	/*!***********************************************************************
	\brief
	 SpriteRender component structure.
	*************************************************************************/
	struct SpriteRender
	{
		std::string texturePath{};				// The path acting as a key to the texture
		SPRITE_SHAPE shape = SPRITE_SHAPE::BOX; // Draw as BOX | Circle
		//GLuint textureID = 0;					// The texture ID, to be set during runtime
		int layer = 0;							// The layer to render the sprite on
		bool animated = false;					// Is the object animated?

		bool flipX = false;
		bool flipY = false;

		Vec3 color{1.f,1.f,1.f};
	};

	/*!***********************************************************************
	\brief
	 Script component structure.
	*************************************************************************/
	struct Script
	{
		std::string scriptPath;
		std::string scriptName;
		void *instance = nullptr;		// MonoObject from client script
		void *handle = nullptr;			// MonoGCHandle from client script
		void *methodInstance = nullptr; // MonoMethod from client script
	};

	/*!***********************************************************************
	\brief
	 Player component structure.
	*************************************************************************/
	struct Player
	{
		int maxHealth = 100;
		int currentHealth = 100;
		int postInjuriesMaxHealth = 100;
		int comboState = 0;			// Tracks current combat state 
		int comboDamage = 10;
		float comboTimer = 0.0f;	// Tracks time since last attack
		float maxComboTimer = 5.0f; // Max time to continue combo
		float hitStopDuration = 0.05f; // Duration of hit stop in seconds
		float hitStopTimer = 0.0f; // Timer for hit stop
		float playerForce = 4500.0f;
		bool isHitStopActive = false; // Flag to check if hit stop is active
		bool isDead = false;
		bool canAttack = true;
		bool comboIsAttacking = false;

		/**
		 * @brief Handle player's attack animation to stop for a certain while
		 */
		void HitStopAnimation()
		{
			hitStopTimer = hitStopDuration;
			isHitStopActive = true;
		}
	};

	/*!***********************************************************************
	\brief
	 Player soul component structure.
	*************************************************************************/
	enum SoulType { EMPTY = -1, FISH, WORM, NUM_OF_SOULS }; // The type of souls
	struct PlayerSoul
	{
		SoulType current_soul = EMPTY;					 // The player's current active soul
		float soul_bars[NUM_OF_SOULS]{};				 // The player's soul bars, used for soul passive attacks
		int soul_charges[NUM_OF_SOULS]{};				 // The player's soul charges, used for soul abilities

		float skill_damages[NUM_OF_SOULS]{ 50.f, 50.f }; // The damage of the skills
		float skill_duration = 1.f;						 // The duration of the skill
		float skill_cooldown = 1.f;						 // The cooldown of the skill
		float skill_timer = 0.f;						 // The timer for skill ready
		bool skill_ready = false;						 // The skill's ready state
		bool is_casting = false;						 // The soul animation casting state

		float soul_decay_amount = 10.f;					 // The amount of soul to decay
		float soul_decay_rate = 5.f;					 // The rate of decay for the soul bar
		float soul_decay_timer = 0.f;					 // The timer for soul decay
	};

	/*!***********************************************************************
	\brief
	 Soul orb component structure.
	*************************************************************************/
	struct SoulOrb
	{
		SoulType orb_type = EMPTY;  // The type of soul orb to harvest
		float soul_amount{};		// The amount of soul to harvest
		float orb_speed{ 600.f };	// The speed of the orb
		Vec3 target_position{};	    // The target position to go to
	};

	/*!***********************************************************************
	\brief
	 Boss component structure.
	*************************************************************************/
	struct Boss
	{
		//not in used
		enum BossStates
		{
			ATTACK1,
			ATTACK2,
			DEAD1,
			DEAD2,

		};
		int BossPhase = 0;
		EntityID ID;
		BossStates state;
		float health;
		float attackPower;
		float waitTime;

		Boss() = default;
		Boss(EntityID ID)
			: ID(ID), state(BossStates::ATTACK1)
		{
			health = 1000.f;
			attackPower = 15.f;
			waitTime = 1000.f;
		}

	};

	/*!***********************************************************************
	\brief
	 Enemy component structure.
	*************************************************************************/
	struct Enemy
	{
		enum EnemyStates
		{
			MOVE,
			STANDBY,
			ATTACK,
			DEAD,
		};

		enum EnemyTypes
		{
			FISH,
			WORM,
			DEFAULT,
		};

		EntityID ID;
		EnemyStates state;
		EnemyTypes type;
		float posX, posY; //place holder to be remove
		float dirX, dirY;
		float magnitude;
		float targetX, targetY;
		float health;
		float attackPower;
		float attackRange;
		float speed;
		int nearestObj;
		int collideObj;
		mutable int prevObject;
		mutable int prevObject2;
		bool isCollide;
		bool isKick;
		float kicktime = 1.f;
		bool hasDealtDamage = false;
		bool wormshoot = false;
		float atktimer = 0.0f;
		bool isAtk = false;
		bool isDead = false;
		bool isWithPlayer = false;
		float timeSinceTargetReached = 0.f;
		bool wasHit = false;  // New flag for hit detection
		float waitTime = 0.f;
		bool iswaiting = false;
		bool isKilled =false;
		bool deadsound = false;
		int move = 5;

		Enemy() = default;

		// Constructor
		Enemy(float startX, float startY, EnemyTypes type, EntityID ID)
			: ID(ID), state(EnemyStates::MOVE), type(type), posX(startX), posY(startY), targetX(startX), targetY(startY), prevObject(-1), prevObject2(-1), isCollide(false), isKick(false)
		{
			wormshoot = false;
			hasDealtDamage = false;
			nearestObj = -1;
			collideObj = -1;
			switch (type)
			{
			case Enemy::FISH:
				health = 80.f;
				attackPower = 15.f;
				attackRange = 300.f;
				speed = 4000.f;
				break;
			case Enemy::WORM:
				health = 60.f;
				attackPower = 5.f;
				attackRange = 700.f;
				speed = 3000.f;
				break;
			case Enemy::DEFAULT:
				break;
			default:
				break;
			}
		}

	static	bool LineIntersectsRect(float startX, float startY, float dirX, float dirY,
			float length, float objX, float objY, float objWidth, float objHeight)
		{
		float minX = objX - 0.5f * objWidth;
		float maxX = objX + 0.5f * objWidth;
		float minY = objY - 0.5f * objHeight;
		float maxY = objY + 0.5f * objHeight;

		// Ensure the start point is INSIDE the rectangle
		if (startX > minX && startX < maxX && startY > minY && startY < maxY)
			return true;

		float tMin = 0.0f, tMax = length;

		// Check intersection along X-axis
		if (dirX != 0.0f)
		{
			float invD = 1.0f / dirX;
			float t0 = (minX - startX) * invD;
			float t1 = (maxX - startX) * invD;
			if (invD < 0.0f) std::swap(t0, t1);
			tMin = std::max(tMin, t0);
			tMax = std::min(tMax, t1);
		}
		else if (startX <= minX || startX >= maxX)
		{
			return false; // Line is vertical but outside X bounds
		}

		// Check intersection along Y-axis
		if (dirY != 0.0f)
		{
			float invD = 1.0f / dirY;
			float t0 = (minY - startY) * invD;
			float t1 = (maxY - startY) * invD;
			if (invD < 0.0f) std::swap(t0, t1);
			tMin = std::max(tMin, t0);
			tMax = std::min(tMax, t1);
		}
		else if (startY <= minY || startY >= maxY)
		{
			return false; // Line is horizontal but outside Y bounds
		}

		// Ensure intersection occurs within the given length and is non-negative
		return (tMin <= tMax) && (tMax >= 0) && (tMin <= length);
	}

		// Check if two points are within a threshold distance
		bool ReachedTarget(float x1, float y1, float x2, float y2, float threshold) const
		{
			float dx = x1 - x2;
			float dy = y1 - y2;
			return (dx * dx + dy * dy) <= (threshold * threshold);
		}

		// Check if two points are within a threshold distance
		void SetTarget(Transform& target)
		{
			this->targetX = target.position.x;
			this->targetY = target.position.y;
		}

		// Set the target destination
		void SetTarget(float newtargetX, float newtargetY)
		{
			this->targetX = newtargetX;
			this->targetY = newtargetY;
		}

		//helper function
		template <typename T>
		T Lerp(T a, T b, float t)
		{
			return a + t * (b - a);
		}
		
		//move to target
		void MoveToTarget(Transform& self, float newtargetX, float newtargetY, float deltaTime, float movespeed)
		{
			// Check if already at the target
			if (ReachedTarget(self.position.x, self.position.y, newtargetX, newtargetY, 0.1f))
			{
				if (!isCollide)
				{
					prevObject = nearestObj;
					nearestObj = -1;
				}
				return;
			}

			float dx = newtargetX - self.position.x;
			float dy = newtargetY - self.position.y;
			float distance = std::sqrt(dx * dx + dy * dy);

			// Normalize direction
			if (distance > 0.0f)
			{
				dx /= distance;
				dy /= distance;
			}

			// Move the enemy
			self.position.x += dx * movespeed * deltaTime;
			self.position.y += dy * movespeed * deltaTime;
			posX = self.position.x;
			posY = self.position.y;
		}

		// Check if the enemy can attack the player
		bool IsPlayerInRange(Transform& player, Transform& enemy) const
		{
			float dx = player.position.x - enemy.position.x;
			float dy = player.position.y - enemy.position.y;

			float distance = dx * dx + dy * dy;

			return distance <= attackRange * attackRange;
		}

		bool IsPlayerInAttackRange(Transform& player, Transform& enemy) const
		{
			float dx = player.position.x - enemy.position.x;
			float dy = player.position.y - enemy.position.y;

			if (dx * dx <= attackRange * attackRange && std::abs(dy) <= 20)
			{
				return true;
			}
			return false;
		}

		//wrap to target when collide
		void WrapToTarget(Transform& enemyTransform, float newtargetX, float newtargetY, float deltaTime, float movespeed)
		{
			// Calculate the direction vector from the enemy to the target
			float dx = newtargetX - enemyTransform.position.x;
			float dy = newtargetY - enemyTransform.position.y;

			// Calculate the current angle to the target
			float currentAngle = std::atan2(dy, dx); // Initialize angle based on initial direction

			// Calculate the current radius (distance to the target)
			float radius = std::sqrt(dx * dx + dy * dy);

			// If close enough to the target, snap to it and stop wrapping
			if (radius < 100.f) // Threshold for "reaching" the target
			{
				isCollide = false;
				nearestObj = -1;
				return;
			}

			// Reduce the radius over time to spiral inwards
			float shrinkRate = movespeed * deltaTime * 0.5f; // Adjust shrink rate for smooth spiraling
			radius = std::max(radius - shrinkRate, 0.1f); // Ensure the radius doesn't go negative

			// Increment the angle dynamically for circular motion
			float angularVelocity = movespeed / (radius + 0.1f); // Adjust angular speed based on the shrinking radius
			currentAngle += angularVelocity * deltaTime;

			// Calculate the new position along the circular path
			float newX = newtargetX - radius * std::cos(currentAngle);
			float newY = newtargetY - radius * std::sin(currentAngle);

			// Smoothly move the enemy's position toward the new point
			enemyTransform.position.x = Lerp(enemyTransform.position.x, newX, deltaTime * movespeed);
			enemyTransform.position.y = Lerp(enemyTransform.position.y, newY, deltaTime * movespeed);
		}



		// Attack the player (reduces player's health)
		void AttackPlayer(int& playerHealth)
		{
			if (playerHealth > 0.0f)
			{
				playerHealth -= static_cast<int>(attackPower);

				if (playerHealth < 0.0f)
				{
					playerHealth = static_cast<int>(0.0f); // Ensure health does not go negative
				}
			}
		}

		// Reduce the enemy's health when taking damage
		void TakeDamage(float damage)
		{
			health -= damage;
			if (health < 0.0f)
			{
				health = 0.0f; // Ensure health does not go negative
			}
			wasHit = true;
			atktimer = 1.5f;
			isCollide = false;
		}
	};

	/*!***********************************************************************
	\brief
	 EnemyBullet component structure.
	*************************************************************************/
	struct EnemyBullet
	{
		float lifetime = 5.0f;
		bool hit = false;
		EnemyBullet() = default;
	};

	/*!***********************************************************************
	\brief
	 AudioSource component structure.
	*************************************************************************/
	struct AudioManager 
	{
		struct AudioSource
		{
			std::string audioPath;
			std::string audioName;
			float volume;

			AudioSource(std::string name, std::string path) :audioName(name), audioPath(path) { volume = 0.2f; }
		};

		std::vector<AudioSource> music; // Music category
		std::vector<AudioSource> sfx;   // Sfx category

		AudioManager() = default;

		//player the music audio
		void PlayMusic(int index) {
			if (index < music.size() && music[index].audioPath != "")
			{
				Audio::GetInstance().LoadSound(index, music[index].audioPath.c_str(), "Music");
				Audio::GetInstance().PlaySound(index, "Music", music[index].volume);
			}
		}
		//stop the music audio
		void StopMusic(int index) {
			Audio::GetInstance().StopSound(index, "Music");
		}

		//player the SFX audio
		void PlaySFX(int index) {
			if (index < sfx.size() && sfx[index].audioPath != "")
			{
				Audio::GetInstance().LoadSound(index, sfx[index].audioPath.c_str(), "SFX");
				Audio::GetInstance().PlaySound(index, "SFX", sfx[index].volume);
			}
		}

		//stop the SFX audio
		void StopSFX(int index) {
			Audio::GetInstance().StopSound(index, "SFX");
		}

		//get the audio index
		int GetSFXindex(const std::string& name) {
			auto it = std::find_if(sfx.begin(), sfx.end(), [&name](const AudioSource& source) {
				return source.audioName == name;
				});

			if (it != sfx.end()) {
				return static_cast<int>(std::distance(sfx.begin(), it)); // Return the index
			}

			std::cerr << "Sound effect with name '" << name << "' not found in SFX category." << std::endl;
			return -1; // Return an invalid index if not found
		}

		//get the audio index
		int GetMusicIndex(const std::string& name) {
			auto it = std::find_if(music.begin(), music.end(), [&name](const AudioSource& source) {
				return source.audioName == name;
				});

			if (it != music.end()) {
				return static_cast<int>(std::distance(music.begin(), it)); // Return the index
			}

			std::cerr << "Music with name '" << name << "' not found in Music category." << std::endl;
			return -1; // Return an invalid index if not found
		}

		//load the sound to audio
		void AddSoundToMusic(const std::string& name) {
			//music.emplace_back(path, name);

			int index = GetMusicIndex(name);
			Audio::GetInstance().LoadSound(index, music[index].audioPath.c_str(), "Music");
		}

		//load the sound to audio
		void AddSoundToSfx(const std::string& name) {
			//sfx.emplace_back(path, name);
			int index = GetSFXindex(name);
			Audio::GetInstance().LoadSound(index, sfx[index].audioPath.c_str(), "SFX");
		}

		// Remove sound by index from the Music category
		void RemoveSoundFromMusic(size_t index) {
			if (index < music.size())
				music.erase(music.begin() + index);
		}

		// Remove sound by index from the SFX category
		void RemoveSoundFromSfx(size_t index) {
			if (index < sfx.size())
				sfx.erase(sfx.begin() + index);
		}

		void SetSFXvolume(float amount)
		{
			for (int i = 0; i < sfx.size(); i++)
			{
				sfx[i].volume += amount;
				if (sfx[i].volume <= 0.0f)
				{
					sfx[i].volume = 0.f;
				}
				sfx[i].volume = std::clamp(sfx[i].volume, 0.0f, 1.0f);
			}
		}

		void SetMusicVolume(float amount)
		{
			for (int i = 0; i < music.size(); i++)
			{
				music[i].volume += amount;
				if (music[i].volume <= 0.0f)
				{
					music[i].volume = 0.f;
				}
				music[i].volume = std::clamp(music[i].volume, 0.0f, 1.0f);
				Audio::GetInstance().UpdateMusicVolume(i, music[i].volume);
			}
		}

		void SetAudioVolume(float amount)
		{
			SetMusicVolume(amount);
			SetSFXvolume(amount);
		}
	};

	/*!***********************************************************************
	\brief
	 VideoSource component structure.
	*************************************************************************/
	struct VideoSource {
		std::string videoPath;  // Path to the video file
		std::string videoName;  // Name of the video

		VideoSource(std::string path, std::string name)
			: videoPath(std::move(path)), videoName(std::move(name)) {}
	};

	/*!***********************************************************************
	\brief
	 VideoData component structure. Contains categorized video sources.
	*************************************************************************/
	struct VideoData {
		std::vector<VideoSource> videos;  // List of videos

		VideoData()
		{
			videos = std::vector<VideoSource>();
		}
	};

}