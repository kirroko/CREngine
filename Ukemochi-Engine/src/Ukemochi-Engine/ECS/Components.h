/* Start Header
*****************************************************************/
/*!
\file		Components.h
\par		Ukemochi
\author		WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu (50%)
\co-authors Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\par		junyukean.wong\@digipen.edu
\par		Course: CSD2400/CSD2401
\date		26/09/24
\brief		Here is where we store all the different components that are needed to be added or removed (i.e Transform, Sprite, etc)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once

#include <glad/glad.h>			// for GLuint
#include "PreCompile.h"
#include "../Math/Vector2D.h"	// for Vec2 struct
#include "../Audio/Audio.h"		// for Audio class
#include "../Graphics/Sprite.h" // for Sprite class

namespace Ukemochi
{
	struct Transform
	{
		Vec2 position;
		float rotation; // TODO: Change to Quaternion?
		Vec2 scale;
	};

	/*!***********************************************************************
	\brief
	 Rigidbody2D component structure.
	*************************************************************************/
	struct Rigidbody2D
	{
		// Linear Properties
		Vec2 position;
		Vec2 velocity;
		Vec2 acceleration;
		Vec2 force;
		float mass;
		float inverse_mass; // inverse mass = 1/mass
		float linear_drag; // Adjust to control the friction from [0, 1]

		// Rotational Properties
		/*float angle;
		float angular_velocity;
		float angular_acceleration;
		float torque;
		float inertia_mass;
		float angular_drag*/

		bool use_gravity; // If true, apply gravity
		bool is_kinematic; // If true, don't apply physics

		/*!***********************************************************************
		\brief
		 Rigidbody2D default constructor.
		*************************************************************************/
		Rigidbody2D() : position{}, velocity{}, acceleration{},
			force{}, mass{ 1.f }, inverse_mass{ 1 / mass }, linear_drag{ 0.9f },
			use_gravity{ false }, is_kinematic{ false } {}

		/*!***********************************************************************
		\brief
		 Rigidbody2D constructor with kinematic argument. For static objects.
		\param[in] kinematic
		 The kinematic to set, if true, don't apply physics.
		*************************************************************************/
		Rigidbody2D(const bool kinematic) : position{}, velocity{}, acceleration{},
			force{}, mass{ 1.f }, inverse_mass{ 1 / mass }, linear_drag{ 0.9f },
			use_gravity{ false }, is_kinematic{ kinematic } {}

		/*!***********************************************************************
		\brief
		 Rigidbody2D constructor with force argument. For dynamic objects.
		\param[in] force
		 The force to set.
		*************************************************************************/
		Rigidbody2D(const Vec2 force) : position{}, velocity{}, acceleration{},
			force{ force }, mass{ 1.f }, inverse_mass{ 1 / mass }, linear_drag{ 0.9f },
			use_gravity{ false }, is_kinematic{ false } {}

		/*!***********************************************************************
		\brief
		 Rigidbody2D constructor with all member arguments. For serialization.
		*************************************************************************/
		Rigidbody2D(const Vec2 pos, const Vec2 vel, const Vec2 accel, const Vec2 force, const float mass,
			const float inv_mass, const float drag, const bool gravity, const bool kinematic)
			: position{ pos }, velocity{ vel }, acceleration{ accel },
			force{ force }, mass{ mass }, inverse_mass{ inv_mass }, linear_drag{ drag },
			use_gravity{ gravity }, is_kinematic{ kinematic } {}
	};

	/*!***********************************************************************
	\brief
	 BoxCollider2D component structure.
	*************************************************************************/
	struct BoxCollider2D
	{
		// Min,max for AABB
		Vec2 min;
		Vec2 max;

		int collision_flag; // Track the collision flags
		bool is_trigger; // If true, act as a trigger
		bool is_player; // temp

		/*!***********************************************************************
		\brief
		 BoxCollider2D default constructor.
		*************************************************************************/
		BoxCollider2D() : min{}, max{}, collision_flag{}, is_trigger{ false }, is_player{ false } {}

		/*!***********************************************************************
		\brief
		 BoxCollider2D constructor with trigger argument. For trigger objects.
		\param[in] trigger
		 Set true, if entity is a trigger.
		*************************************************************************/
		BoxCollider2D(const bool trigger) : min{}, max{}, collision_flag{}, is_trigger{ trigger }, is_player{ false } {}

		/*!***********************************************************************
		\brief
		 BoxCollider2D constructor with trigger and player arguments. TEMP
		\param[in] player
		 Set true, if entity is a player.
		*************************************************************************/
		BoxCollider2D(const bool trigger, const bool player) : min{}, max{}, collision_flag{}, is_trigger{ trigger }, is_player{ player } {}

		/*!***********************************************************************
		\brief
		 BoxCollider2D constructor with all member arguments. For serialization.
		*************************************************************************/
		BoxCollider2D(const Vec2 min, const Vec2 max, const int flag, const bool trigger, const bool player)
			: min{ min }, max{ max }, collision_flag{ flag }, is_trigger{ trigger }, is_player{ player } {}
	};

	/*!***********************************************************************
	\brief
	 CircleCollider2D component structure.
	*************************************************************************/
	struct CircleCollider2D
	{
		Vec2 m_center;
		float m_radius;

		/*!***********************************************************************
		\brief
		 CircleCollider2D default constructor.
		*************************************************************************/
		CircleCollider2D() : m_center{}, m_radius{} {}

		/*!***********************************************************************
		\brief
		 CircleCollider2D constructor with all member arguments. For serialization.
		*************************************************************************/
		CircleCollider2D(const Vec2 center, const float radius) : m_center{ center }, m_radius{ radius } {}
	};

	enum class SPRITE_SHAPE { BOX = 0, CIRCLE = 1 }; // Enum for the shape of the sprite, THIS FOLLOWS THE GRAPHICS RENDERER

	struct SpriteRender
	{
		std::string texturePath{};				// Path to the texture file (sprite)
		SPRITE_SHAPE shape = SPRITE_SHAPE::BOX;	// Draw as BOX | Circle
		unsigned int textureID = 0;
		bool animated = false;							// Is the object animated?

		//bool visible = true;							// Is the object visible?
		//float opacity = 1.0f;							// Opacity of the object
	};

	struct AudioSource
	{
		//FMOD::Sound pSounds;
		//FMOD::Channel pChannels;
		//FMOD::ChannelGroup pChannelGroups;
	};
}