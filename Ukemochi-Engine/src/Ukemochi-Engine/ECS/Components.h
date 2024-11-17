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

	struct SpriteRender
	{
		std::string texturePath{};				// Path to the texture file (sprite)
		SPRITE_SHAPE shape = SPRITE_SHAPE::BOX;	// Draw as BOX | Circle
		unsigned int textureID = 0;
		bool animated = false;							// Is the object animated?

		int animationIndex = -1;
		bool flipX = false;
		bool flipY = false;
		//bool visible = true;							// Is the object visible?
		//float opacity = 1.0f;							// Opacity of the object
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
}