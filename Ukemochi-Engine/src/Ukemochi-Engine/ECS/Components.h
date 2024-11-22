/* Start Header ************************************************************************/
/*!
\file       Components.h
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu (50%)
\co-authors Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu (25%)
\date       Nov 17, 2024
\brief      Here is where we store all the different components that are needed to be added or removed (i.e Transform, Sprite, etc).

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include <glad/glad.h>			// for GLuint
#include "PreCompile.h"
#include "../Math/Matrix4x4.h"  // for Mtx44 struct
#include "../Math/Vector2D.h"	// for Vec2 struct
#include "../Audio/Audio.h"		// for Audio class

//for enemy
#include "../FrameController.h"

namespace Ukemochi
{
	/*!***********************************************************************
	\brief
	 Transform component structure.
	*************************************************************************/
	struct Transform
	{
		Mtx44 transform_matrix;
		Vec2 position;
		float rotation = 0.f; // TODO: Change to Quaternion?
		Vec2 scale;
	};

	/*!***********************************************************************
	\brief
	 Rigidbody2D component structure.
	*************************************************************************/
	struct Rigidbody2D
	{
		// Linear Properties
		Vec2 position{};
		Vec2 velocity{};
		Vec2 acceleration{};
		Vec2 force{};
		float mass{ 1.f };							  // Minimum mass of 1
		float inverse_mass{ 1.f / mass };			  // inverse mass = 1/mass
		float linear_drag{ 0.9f };					  // Adjust to control the friction from [0, 1]

		// Rotational Properties
		float angle{};
		float angular_velocity{};
		float angular_acceleration{};
		float torque{};
		float inertia_mass{ 1.f };					  // Minimum inertia mass of 1
		float inv_inertia_mass{ 1.f / inertia_mass }; // inverse inertia mass = 1/inertia mass
		float angular_drag{ 0.9f };					  // Adjust to control the friction from [0, 1]

		bool use_gravity{ false };					  // If true, apply gravity
		bool is_kinematic{ false };					  // If true, don't apply physics
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

		int collision_flag{};		   // Track the collision flags
		bool is_trigger{ false };	   // If true, act as a trigger
		//bool enabled{ true };		   // If true, box collision is enabled
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
	 GUI Text component structure.
	*************************************************************************/
	//struct Text
	//{
	//	//std::string label;	   // The text on the textbox
	//	//Vec2 initial_pos{};	   // The initial position of the textbox
	//	//std::string alignment; // The text alignment (left, middle, right)
	//	//std::string font;	   // The text font
	//	//int font_size;		   // The text font size
	//};

	/*!***********************************************************************
	\brief
	 GUI Button component structure.
	*************************************************************************/
	//struct Button
	//{
	//	//std::function<void()> on_click; // The event to trigger on click
	//	//bool interactable{ true };		// Is the button interactable
	//	
	//	//std::string label{};			// The text on the button
	//	//Vec2 initial_pos{};				// The initial position of the button
	//};

	/*!***********************************************************************
	\brief
	 SpriteRender component structure.
	*************************************************************************/
	enum class SPRITE_SHAPE { BOX = 0, CIRCLE = 1 }; // Enum for the shape of the sprite, THIS FOLLOWS THE GRAPHICS RENDERER

	struct AnimationClip
	{
		std::string keyPath{};
		std::string name{};			// Name of the animation clip
		GLuint textureID = 0;		// The texture ID, to be set during runtime
		int total_frames = 1;				// Total frames in the animation
		int pixel_width = 64;				// Width of each frame in pixels
		int pixel_height = 64;				// Height of each frame in pixels
		int total_width = 64;				// Width of the sprite sheet
		int total_height = 64;				// Height of the sprite sheet
		float frame_time = 0.05f;	// Duration of each frame
		bool looping = true;		// Should the animation loop?

		// AnimationClip();
		// AnimationClip(std::string name, GLuint textureID, int total_frames, int pixel_width, int pixel_height, int total_width, int total_height, float frameTime, bool looping)
		// 	: name(std::move(name)), textureID(textureID), total_frames(total_frames), pixel_width(pixel_width), pixel_height(pixel_height), total_width(total_width), total_height(total_height), frame_time(frameTime), looping(looping) {}
	};
	
	/*!
	 * @struct Animation
	 * @brief Manages frame-based animations for entities.
	 */
	struct Animation
	{
		std::unordered_map<std::string, AnimationClip> clips;	// Animation clips
		std::string currentClip{};								// Name of the active animation.
		std::string defaultClip{};								// Name of the default animation.
		int current_frame = 0;									// Current frame index
		int original_frame = 0;									// Original frame index
		float time_since_last_frame = 0.0f;						// Time since the last frame
		float original_frame_time = 0.05f;						// Original frame time
		bool frame_changed_flag = false;						// Flag to check if the total_frame has changed
		bool play_Uninterrupted = false;						// Play the animation without interruption
		bool is_playing = true;									// Is the animation playing?
		
		bool SetAnimation(const std::string& name)
		{
			if(clips.find(name) != clips.end() && name != currentClip)
			{
				currentClip = name;
				current_frame = 0;

				if (frame_changed_flag)
				{
					clips[name].total_frames = original_frame;
					frame_changed_flag = false;
				}

				time_since_last_frame = 0.0f;

				return true;
			}
			return false;
		}

		bool SetAnimation(const std::string& name, int startFrame, int endFrame)
		{
			if (clips.find(name) != clips.end())
			{
				currentClip = name;
				current_frame = startFrame;
				if (!frame_changed_flag)
				{
					original_frame = clips[name].total_frames;
					clips[name].total_frames = endFrame;
					frame_changed_flag = true;
				}
				else
				{
					clips[name].total_frames = endFrame;
				}

				return true;
			}

			return false;
		}
		
		void update(float dt)
		{
			if(clips.find(currentClip) == clips.end()) // Don't update if the clip doesn't exist
				return;

			if (!is_playing) // Don't update if the animation is set not to play
				return;

			AnimationClip& clip = clips[currentClip];
			time_since_last_frame += dt;

			// Advance new frame
			if(time_since_last_frame >= clip.frame_time)
			{
				current_frame++;
				// time_since_last_frame -= clip.frame_time;
				if(current_frame >= clip.total_frames)
				{
					current_frame = clip.looping ? 0 : clip.total_frames - 1;
					// current_frame = clip.looping ? 0 : SetAnimation(defaultClip);
				}
				time_since_last_frame = 0.0f; // Reset time
			}
			// Renderer system handles the UV coordinates for us
		}

		void setCurrentClipDuration(float newDuration)
		{
			if(clips.find(currentClip) == clips.end())
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
	};
	
	struct SpriteRender
	{
		std::string texturePath{};					// The path acting as a key to the texture
		SPRITE_SHAPE shape = SPRITE_SHAPE::BOX;		// Draw as BOX | Circle
		GLuint textureID = 0;						// The texture ID, to be set during runtime
		bool animated = false;						// Is the object animated?
		
		bool flipX = false;
		bool flipY = false;
	};
	
	/*!***********************************************************************
	\brief
	 AudioSource component structure.
	*************************************************************************/
	struct AudioSource
	{
		FMOD::Sound pSounds;
		FMOD::Channel pChannels;
		FMOD::ChannelGroup pChannelGroups;
	};

	/*!***********************************************************************
	\brief
	 Script component structure.
	*************************************************************************/
	struct Script
	{
		std::string scriptPath;
		std::string scriptName;
		void* instance = nullptr; // MonoObject from client script
		void* handle = nullptr; // MonoGCHandle from client script
		void* methodInstance = nullptr; // MonoMethod from client script
	};

	/*!***********************************************************************
	\brief
	 Enemy component structure.
	*************************************************************************/
	struct Enemy
	{
		enum EnemyStates
		{
			ROAM,
			CHASE,
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
		float posX, posY;
		float targetX, targetY;
		float health;
		float attackPower;
		float attackRange;
		float speed;
		int nearestObj;
		mutable int prevObject;
		bool isCollide;

		// Check if two points are within a threshold distance
		bool ReachedTarget(float x1, float y1, float x2, float y2, float threshold) const {
			float dx = x1 - x2;
			float dy = y1 - y2;
			return (dx * dx + dy * dy) <= (threshold * threshold);
		}

		EnemyStates GetEnemyState() const
		{
			return state;
		}

		void SetEnemyState(EnemyStates newState)
		{
			state = newState;
		}

		bool IsCollide() const
		{
			return isCollide;
		}

		void SetIsCollide(bool b)
		{
			isCollide = b;
		}
		Enemy() = default;
		// Constructor
		Enemy(float startX, float startY, EnemyTypes type, EntityID ID)
			: ID(ID), state(EnemyStates::ROAM), type(type), posX(startX), posY(startY), targetX(startX), targetY(startY), prevObject(-1), isCollide(false)
		{
			nearestObj = -1;
			switch (type)
			{
			case Enemy::FISH:
				health = 120.f;
				attackPower = 20.f;
				attackRange = 0.5f;
				speed = 150.f;
				break;
			case Enemy::WORM:
				health = 100.f;
				attackPower = 10.f;
				attackRange = 5.f;
				speed = 150.f;
				break;
			case Enemy::DEFAULT:
				break;
			default:
				break;
			}
		}


		void RoamState(Transform& self,std::vector<EntityID>& environmentObjects ,Transform& nearestObjTransform, int playerID, Transform& playerTransform)
		{
			float targetX = nearestObjTransform.position.x;
			float targetY = nearestObjTransform.position.y;

			// Move to the nearest object
			MoveToTarget(self,targetX, targetY, g_FrameRateController.GetDeltaTime(), speed);

			//when target reach find next obj
			if (ReachedTarget(GetPosition().first, GetPosition().second, targetX, targetY, 0.f) == true)
			{
				nearestObj = -1;
			}

			if (playerID == -1)
			{
				return;
			}

			if (ReachedTarget(GetPosition().first, GetPosition().second,
				playerTransform.position.x,
				playerTransform.position.y, 250.f) == true)
			{
				state = CHASE;
				return;
			}

		}

		void ChaseState(Transform& self,Transform& player)
		{
			SetTarget(player);

			MoveToTarget(self,player.position.x,
				player.position.y, g_FrameRateController.GetDeltaTime(), speed);

			if (ReachedTarget(GetPosition().first, GetPosition().second,
				player.position.x,
				player.position.y, 250.f) == false)
			{
				state = ROAM;
				return;
			}

			//in attack range
			if (IsPlayerInRange(player))
			{
				state = ATTACK;
				return;
			}
		}

		void AttackState()
		{
			static float timer = 0.0f;

			timer -= g_FrameRateController.GetDeltaTime();

			if (timer <= 0.0f)
			{
				//AttackPlayer(GameObjectManager::GetInstance().GetGOByTag("Player")->GetComponent<Collision>());
				timer = 3.0f;
			}
		}

		void SetTarget(Transform& target)
		{
			this->targetX = target.position.x;
			this->targetY = target.position.y;
		}

		// Set the target destination
		void SetTarget(float targetX, float targetY) {
			this->targetX = targetX;
			this->targetY = targetY;
		}

		//GET TRANSFORM TO MOVE
		void MoveToTarget(Transform& self,float targetX, float targetY, float deltaTime, float speed)
		{
			// Check if already at the target
			if (ReachedTarget(self.position.x, self.position.y, targetX, targetY, 0.1f))
			{
				std::cout << "Enemy already at the target position.\n";
				return;
			}

			float dx = targetX - self.position.x;
			float dy = targetY - self.position.y;
			float distance = std::sqrt(dx * dx + dy * dy);

			// Normalize direction
			if (distance > 0.0f) {
				dx /= distance;
				dy /= distance;
			}

			// Move the enemy
			self.position.x += dx * speed * deltaTime;
			self.position.y += dy * speed * deltaTime;
			posX = self.position.x;
			posY = self.position.y;
			// Print position for debugging
			std::cout << "Enemy Position: (" << posX << ", " << posY << ")" << std::endl;
		}


		// Check if the enemy can attack the player
		bool IsPlayerInRange(Transform& player) const {
			float playerX = player.position.x;
			float playerY = player.position.y;
			float dx = playerX - posX;
			float dy = playerY - posY;
			float distance = std::sqrt(dx * dx + dy * dy);
			return distance <= attackRange;
		}

		// Attack the player (reduces player's health)
		void AttackPlayer(float& playerHealth) {
			if (playerHealth > 0.0f) {
				playerHealth -= attackPower;
				if (playerHealth < 0.0f) {
					playerHealth = 0.0f; // Ensure health does not go negative
				}
			}
		}

		// Get the current position of the enemy
		std::pair<float, float> GetPosition() const {
			return { posX, posY };
		}

		// Get the current health of the enemy
		float GetHealth() const {
			return health;
		}

		// Reduce the enemy's health when taking damage
		void TakeDamage(float damage) {
			health -= damage;
			if (health < 0.0f) {
				health = 0.0f; // Ensure health does not go negative
			}
		}
	};
}