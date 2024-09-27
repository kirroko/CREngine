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
#include "../Math/Vector2D.h" // for Vec2 struct

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
		Vec2 position_curr{};
		Vec2 position_prev{};
		Vec2 velocity{};
		Vec2 acceleration{ 7.f, 7.f };
		Vec2 scale{ 100.f, 100.f };

		float direction{};
		float angle{};
		float angular_velocity{};
		float angular_acceleration{};

		bool active{ true };
	};

	struct CircleCollider2D
	{
		Vec2 m_center;
		float m_radius;
	};

	struct BoxCollider2D
	{
		Vec2 min;
		Vec2 max;
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