/* Start Header
*****************************************************************/
/*!
\file       Rigidbody2D.h
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Sept 16, 2024
\brief      This file contains the declaration of the Rigidbody2D class.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#include "../ECS/Systems.h" // Inherit from System
#include "../Math/Vector2D.h" // for Vector2D
#include "../Math/Matrix3x3.h" // for Matrix3x3
#include "../Collision/BoxCollision2D.h" // temp for testing collision

namespace Ukemochi
{
	class PhysicsSystem : public System
	{
	public: // private:
		Mtx33 transform{};
		Vec2 position{};
		Vec2 velocity{};
		Vec2 acceleration{};

		double direction{};
		double angle{};
		double angular_velocity{};
		double angular_acceleration{};

		AABB bounding_box{};
		Vec2 initial_position{};
		Vec2 position_prev{};
		Vec2 scale{};
		bool active{};

	public:
		PhysicsSystem()
			: transform(Mtx33{}), position(Vec2{}), velocity(Vec2{}), direction(0.0) {}
		PhysicsSystem(Mtx33 trans, Vec2 pos, Vec2 vel, double dir)
			: transform(trans), position(pos), velocity(vel), direction(dir) {}
	};

	PhysicsSystem rb;

	void UpdateRigidbody();
}