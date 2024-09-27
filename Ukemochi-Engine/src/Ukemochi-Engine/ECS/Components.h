/* Start Header
*****************************************************************/
/*!
\file		Components.h
\par		Ukemochi
\author		WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu (50%)
\co-authors
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
#include "../Math/Vector2D.h"

namespace Ukemochi
{
	struct Transform
	{
		Vec2 position;
		Vec2 rotation;
		Vec2 scale;
	};

	struct Rigidbody2D
	{
		// Vec2 position
		// Vec2 velocity
		// Vec2 acceleration
		//
		double direction;
		double angle;
		double angular_velocity;
		double angular_acceleration;

		// Vec2 initial_position
		// Vec2 position_prev
		// Vec2 scale
		bool active;
	};

	struct CircleCollider2D
	{
		// vec2 m_center;
		float m_radius;
	};

	struct BoxCollider2D
	{
		// vec2 min
		// vec2 max
	};

	typedef enum { BOX, CIRCLE } SPRITE_SHAPE;

	struct SpriteRender
	{
		std::string texturePath;				// Path to the texture file (sprite)
		SPRITE_SHAPE shape;						// Draw as BOX | Circle

		bool visible;							// Is the object visible?
		float opacity;							// Opacity of the object
	};
}