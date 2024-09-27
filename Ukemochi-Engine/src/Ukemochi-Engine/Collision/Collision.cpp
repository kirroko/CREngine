/* Start Header
*****************************************************************/
/*!
\file       Collision.cpp
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Sept 24, 2024
\brief      This file contains the definition of the Collision system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "PreCompile.h"
#include "Collision.h"			// for forward declaration
#include "../Math/MathUtils.h"  // for min, max, abs
#include "../FrameController.h" // for GetDeltaTime

namespace Ukemochi
{
	void Collision::CheckCollisions()
	{
		//// if the entity has a box collider
		//if (list_of_entities[i] has box collider)

		//if (CollisionIntersection_BoxBox())
		//    ResolveCollisions();

		//if (CollisionIntersection_CircleBox())
		//    ResolveCollisions();

		//if (CollisionIntersection_CircleCircle())
		//    ResolveCollisions();
	}

	void Collision::ResolveCollisions()
	{
	}

	/*!***********************************************************************
	\brief
	 Implementation of collision detection between two boxes.
	\param[in] box1
	 The bounding box of the first box.
	\param[in] vel1
	 The velocity of the first box.
	\param[in] box2
	 The bounding box of the second box.
	\param[in] vel2
	 The velocity of the second box.
	\param[out] firstTimeOfCollision
	 Output variable for the calculated "tFirst" value.
	\return
	 True if there is an intersection, false otherwise.
	*************************************************************************/
	bool Collision::CollisionIntersection_BoxBox(const BoxCollider2D& box1,
		const Vec2& vel1,
		const BoxCollider2D& box2,
		const Vec2& vel2,
		float& firstTimeOfCollision)
	{
		// Step 1: Check for static collision detection between boxes
		if (box1.max.x < box2.min.x || box1.min.x > box2.max.x ||
			box1.max.y < box2.min.y || box1.min.y > box2.max.y)
		{
			// No overlapping boxes
			// return 0;

			// Step 2: Initialize and calculate the new velocity of Vb
			firstTimeOfCollision = 0.0f;
			float tLast = static_cast<float>(UME::g_FrameRateController.GetDeltaTime());
			//float tLast = (float)AEFrameRateControllerGetFrameTime(); // NEED ACCESS TO FRAME TIME!!
			Vec2 vRel = { vel2.x - vel1.x, vel2.y - vel1.y };

			// Step 3: Working with one dimension (x-axis)
			if (vRel.x < 0)
			{
				// Case 1
				if (box1.min.x > box2.max.x)
					return 0; // No intersection and B is moving away

				// Case 4
				if (box1.max.x < box2.min.x)
					firstTimeOfCollision = max((box1.max.x - box2.min.x) / vRel.x, firstTimeOfCollision);
				if (box1.min.x < box2.max.x)
					tLast = min((box1.min.x - box2.max.x) / vRel.x, tLast);
			}
			else if (vRel.x > 0)
			{
				// Case 2
				if (box1.min.x > box2.max.x)
					firstTimeOfCollision = max((box1.min.x - box2.max.x) / vRel.x, firstTimeOfCollision);
				if (box1.max.x > box2.min.x)
					tLast = min((box1.max.x - box2.min.x) / vRel.x, tLast);

				// Case 3
				if (box1.max.x < box2.min.x)
					return 0; // No intersection and B is moving away
			}
			else if (vRel.x == 0)
			{
				// Case 5
				if (box1.max.x < box2.min.x || box1.min.x > box2.max.x)
					return 0; // B is moving parallel to the opposite coordinate axis
			}

			// Case 6
			if (firstTimeOfCollision > tLast)
				return 0; // No intersection

			// Step 4: Working with one dimension (y-axis)
			if (vRel.y < 0)
			{
				// Case 1
				if (box1.min.y > box2.max.y)
					return 0; // No intersection and B is moving away

				// Case 4
				if (box1.max.y < box2.min.y)
					firstTimeOfCollision = max((box1.max.y - box2.min.y) / vRel.y, firstTimeOfCollision);
				if (box1.min.y < box2.max.y)
					tLast = min((box1.min.y - box2.max.y) / vRel.y, tLast);
			}
			else if (vRel.y > 0)
			{
				// Case 2
				if (box1.min.y > box2.max.y)
					firstTimeOfCollision = max((box1.min.y - box2.max.y) / vRel.y, firstTimeOfCollision);
				if (box1.max.y > box2.min.y)
					tLast = min((box1.max.y - box2.min.y) / vRel.y, tLast);

				// Case 3
				if (box1.max.y < box2.min.y)
					return 0; // No intersection and B is moving away
			}
			else if (vRel.y == 0)
			{
				// Case 5
				if (box1.max.y < box2.min.y || box1.min.y > box2.max.y)
					return 0; // B is moving parallel to the opposite coordinate axis
			}

			// Case 6
			if (firstTimeOfCollision > tLast)
				return 0; // No intersection

			// Step 5: Return true: the boxes intersect
			return 1;
		}
		else
		{
			// Overlapping boxes
			return 1;
		}
	}

	/*!***********************************************************************
	\brief
	 Implementation of collision detection between two circles.
	\param[in] circle1
	 The first circle.
	\param[in] circle2
	The second circle.
	\return
	 True if the circles intersect, false otherwise.
	*************************************************************************/
	bool Collision::CollisionIntersection_CircleCircle(const CircleCollider2D& circle1, const CircleCollider2D& circle2)
	{
		// Get the distance between the circles
		float dx{ circle2.m_center.x - circle1.m_center.x };
		float dy{ circle2.m_center.y - circle1.m_center.y };
		float distance_squared{ dx * dx + dy * dy };

		// Check if the circles is intersecting
		return distance_squared <= (circle1.m_radius + circle2.m_radius) * (circle1.m_radius + circle2.m_radius);
	}

	/*!***********************************************************************
	\brief
	 Implementation of collision detection between a circle and a box.
	\param[in] circle
	 The circle to collide.
	\param[in] box
	The box to collide.
	\return
	 True if the circle and box intersect, false otherwise.
	*************************************************************************/
	bool Collision::CollisionIntersection_CircleBox(const CircleCollider2D& circle, const BoxCollider2D& box)
	{
		// Get the distance between the circle and the box
		float dx = abs(circle.m_center.x - (box.min.x + box.max.x) / 2);
		float dy = abs(circle.m_center.y - (box.min.y + box.max.y) / 2);

		// Check if circle center is inside the box
		if (dx <= (box.max.x - box.min.x) / 2 && dy <= (box.max.y - box.min.y) / 2)
			return true;

		// Check if circle is intersecting any of the box's edges
		double distance_squared = dx * dx + dy * dy;
		double radius_squared = circle.m_radius * circle.m_radius;
		return distance_squared <= radius_squared;
	}

	/*!***********************************************************************
	\brief
	 Implementation of collision detection between a box and the screen boundaries.
	\param[in] box
	 The box to collide.
	\param[in] screen_width
	 The width of the game screen.
	\param[in] screen_height
	 The height of the game screen.
	\return
	 True if the box and screen boundary intersect, false otherwise.
	*************************************************************************/
	bool Collision::CollisionIntersection_BoxScreen(const BoxCollider2D& box, const int screen_width, const int screen_height)
	{
		// might require +-camera delta if the camera is movable
		//if (box.min.x < -cameraDelta.x)

		// Check left screen boundary
		if (box.min.x < 0)
		{
			std::cout << "Collision with the left screen boundary\n";
			return true;
			// rb.position.x = rb.scale.x * 0.5f;
			// rb.direction.x = -rb.direction.x;
		}

		// Check right screen boundary
		if (box.max.x > screen_width)
		{
			std::cout << "Collision with the right screen boundary\n";
			return true;
			// rb.position.x = screen_width - rb.scale.x * 0.5f;
			// rb.direction.x = -rb.direction.x;
		}

		// Check top screen boundary
		if (box.min.y < 0)
		{
			std::cout << "Collision with the bottom screen boundary\n";
			return true;
			// rb.position.y = screen_height - rb.scale.y * 0.5f;
			// rb.direction.y = -rb.direction.y;
		}

		// Check bottom screen boundary
		if (box.max.y > screen_height)
		{
			std::cout << "Collision with the top screen boundary\n";
			return true;
			// rb.position.y = rb.scale.y * 0.5f;
			// rb.direction.y = -rb.direction.y;
		}

		// No collision with any boundary
		return false;
	}
}