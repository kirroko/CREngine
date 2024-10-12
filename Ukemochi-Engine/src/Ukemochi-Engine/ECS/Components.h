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
		Vec2 velocity{};
		Vec2 acceleration{};

		bool is_kinematic{};

		// bool use_gravity{};
		/*float direction{};
		float angle{};
		float angular_velocity{};
		float angular_acceleration{};*/
	};

	/*!***********************************************************************
	\brief
	 BoxCollider2D component structure.
	*************************************************************************/
	struct BoxCollider2D
	{
		Vec2 min{};
		Vec2 max{};

		int collision_flag{};
		bool is_player{};
		bool is_trigger{};
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