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

#include "PreCompile.h"
#include "../Math/Vector2D.h" // for Vec2 struct
#include "../Audio/Audio.h"

namespace Ukemochi
{
	struct Transform
	{
		Vec2 position;
		Vec2 rotation;
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

	typedef enum { BOX, CIRCLE } SPRITE_SHAPE;

	struct SpriteRender
	{
		std::string texturePath;				// Path to the texture file (sprite)
		SPRITE_SHAPE shape;						// Draw as BOX | Circle

		bool visible;							// Is the object visible?
		float opacity;							// Opacity of the object
	};

	struct AudioSource
	{
		//FMOD::Sound pSounds;
		//FMOD::Channel pChannels;
		//FMOD::ChannelGroup pChannelGroups;
	};
}