/* Start Header ************************************************************************/
/*!
\file       Components.h
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu (50%)
\co-authors Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu (25%)
\date       Nov 13, 2024
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
//#include "../Graphics/Sprite.h" // for Sprite class

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
		std::string tag{ "Untagged" }; // temp
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
	struct Button
	{
		std::function<void()> on_click; // The event to trigger on click
		bool interactable{ true };		// Is the button interactable
		
		//std::string label{};			// The text on the button
		//Vec2 initial_pos{};				// The initial position of the button
	};

	/*!***********************************************************************
	\brief
	 SpriteRender component structure.
	*************************************************************************/
	enum class SPRITE_SHAPE { BOX = 0, CIRCLE = 1 }; // Enum for the shape of the sprite, THIS FOLLOWS THE GRAPHICS RENDERER

	struct AnimationClip
	{
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
		int current_frame = 0;									// Current frame index
		float time_since_last_frame = 0.0f;						// Time since the last frame
		float original_frame_time = 0.05f;						// Original frame time

		bool SetAnimation(const std::string& name)
		{
			if(clips.find(name) != clips.end() && name != currentClip)
			{
				currentClip = name;
				current_frame = 0;
				time_since_last_frame = 0.0f;

				return true;
			}
			return false;
		}
		
		void update(float dt)
		{
			if(clips.find(currentClip) == clips.end())
				return;
			

			AnimationClip& clip = clips[currentClip];
			time_since_last_frame += dt;

			// Advance new frame
			if(time_since_last_frame >= clip.frame_time)
			{
				current_frame++;
				// time_since_last_frame -= clip.frame_time;
				if(current_frame >= clip.total_frames)
					current_frame = clip.looping ? 0 : clip.total_frames - 1;
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

	struct Script
	{
		std::string scriptPath;
		std::string scriptName;
		void* instance = nullptr; // MonoObject from client script
		void* handle = nullptr; // MonoGCHandle from client script
		void* methodInstance = nullptr; // MonoMethod from client script
	};
}