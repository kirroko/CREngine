/* Start Header
*****************************************************************/
/*!
\file       Collision.cpp
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Sept 29, 2024
\brief      This file contains the definition of the Collision system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "PreCompile.h"
#include "Collision.h"			  // for forward declaration
#include "../Math/MathUtils.h"    // for min, max, abs
#include "../FrameController.h"   // for GetDeltaTime
#include "../ECS/ECS.h"			  // for entity components
#include "../Graphics/Renderer.h" // for Renderer declaration
#include "../Audio/Audio.h"		  // for Audio sound effects

namespace Ukemochi
{
	/*!***********************************************************************
	\brief
	 Check the collisions of all the entities.
	*************************************************************************/
	void Collision::CheckCollisions()
	{
		// Get the screen width and height
		auto renderer_system = ECS::GetInstance().GetSystem<Renderer>();
		screen_width = renderer_system->screen_width;
		screen_height = renderer_system->screen_height;

		for (auto& entity : m_Entities)
		{
			// Get references of the first entity components
			auto& trans = ECS::GetInstance().GetComponent<Transform>(entity);
			auto& rb = ECS::GetInstance().GetComponent<Rigidbody2D>(entity);
			auto& box = ECS::GetInstance().GetComponent<BoxCollider2D>(entity);

			for (auto& entity2 : m_Entities)
			{
				if (entity == entity2)
					continue;

				// Get references of the second entity components
				auto& trans2 = ECS::GetInstance().GetComponent<Transform>(entity2);
				auto& rb2 = ECS::GetInstance().GetComponent<Rigidbody2D>(entity2);
				auto& box2 = ECS::GetInstance().GetComponent<BoxCollider2D>(entity2);

				// Check collision between entities
				float tLast;
				if (CollisionIntersection_BoxBox(box, rb.velocity, box2, rb2.velocity, tLast))
				{
					CollisionResponse_BoxBox(box, box2, trans, trans2, rb, rb2);

					// Play a sound effect on collision
					if (!Audio::GetInstance().IsPlaying(HIT))
						Audio::GetInstance().PlaySoundInGroup(AudioList::HIT, ChannelGroups::LEVEL1);
				}
			}

			// Check collision between entities and the screen boundaries
			if (CollisionIntersection_BoxScreen(box))
				CollisionResponse_BoxScreen(box, trans, rb);
		}
	}

	/*!***********************************************************************
	\brief
	 Implementation of collision detection between two boxes.
	\param[in/out] box1
	 The bounding box of the first box.
	\param[in] vel1
	 The velocity of the first box.
	\param[in/out] box2
	 The bounding box of the second box.
	\param[in] vel2
	 The velocity of the second box.
	\param[out] firstTimeOfCollision
	 Output variable for the calculated "tFirst" value.
	\return
	 True if there is an intersection, false otherwise.
	*************************************************************************/
	bool Collision::CollisionIntersection_BoxBox(BoxCollider2D& box1, const Vec2& vel1, BoxCollider2D& box2, const Vec2& vel2, float& firstTimeOfCollision)
	{
		// Create collision flags for both boxes
		int box1_flag = 0;
		int box2_flag = 0;

		// Step 1: Check for static collision detection between boxes
		if (box1.max.x < box2.min.x || box1.min.x > box2.max.x ||
			box1.max.y < box2.min.y || box1.min.y > box2.max.y)
		{
			// No overlapping boxes
			// return 0;

			// Step 2: Initialize and calculate the new velocity of Vb
			firstTimeOfCollision = 0.0f;
			float tLast = static_cast<float>(Ukemochi::g_FrameRateController.GetDeltaTime());
			Vec2 vRel = { vel2.x - vel1.x, vel2.y - vel1.y };

			// Step 3: Working with one dimension (x-axis)
			if (vRel.x < 0)
			{
				// Case 1: Moving left
				if (box1.min.x > box2.max.x)
					return 0; // No intersection and B is moving away

				// Case 4
				if (box1.max.x < box2.min.x)
				{
					firstTimeOfCollision = max((box1.max.x - box2.min.x) / vRel.x, firstTimeOfCollision);

					box1_flag |= COLLISION_RIGHT;
					box2_flag |= COLLISION_LEFT;
				}
				if (box1.min.x < box2.max.x)
					tLast = min((box1.min.x - box2.max.x) / vRel.x, tLast);
			}
			else if (vRel.x > 0)
			{
				// Case 2: Moving right
				if (box1.min.x > box2.max.x)
				{
					firstTimeOfCollision = max((box1.min.x - box2.max.x) / vRel.x, firstTimeOfCollision);

					box1_flag |= COLLISION_LEFT;
					box2_flag |= COLLISION_RIGHT;
				}
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
				// Case 1: Moving down
				if (box1.min.y > box2.max.y)
					return 0; // No intersection and B is moving away

				// Case 4
				if (box1.max.y < box2.min.y)
				{
					firstTimeOfCollision = max((box1.max.y - box2.min.y) / vRel.y, firstTimeOfCollision);

					box1_flag |= COLLISION_BOTTOM;
					box2_flag |= COLLISION_TOP;
				}
				if (box1.min.y < box2.max.y)
					tLast = min((box1.min.y - box2.max.y) / vRel.y, tLast);
			}
			else if (vRel.y > 0)
			{
				// Case 2: Moving up
				if (box1.min.y > box2.max.y)
				{
					firstTimeOfCollision = max((box1.min.y - box2.max.y) / vRel.y, firstTimeOfCollision);

					box1_flag |= COLLISION_TOP;
					box2_flag |= COLLISION_BOTTOM;
				}
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

			// Set the collision flags to the boxes
			box1.collision_flag = box1_flag;
			box2.collision_flag = box2_flag;

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
	 Collision response for collision between two boxes.
	\param[in] box1
	 The first box that collided.
	\param[in] box2
	 The second box that collided.
	\param[out] trans1
	 The transform component attached to the first collided object.
	\param[out] trans2
	 The transform component attached to the second collided object.
	\param[out] rb1
	 The rigidbody component attached to the first collided object.
	\param[out] rb2
	 The rigidbody component attached to the second collided object.
	*************************************************************************/
	void Collision::CollisionResponse_BoxBox(const BoxCollider2D& box1, const BoxCollider2D& box2, Transform& trans1, Transform& trans2, Rigidbody2D& rb1, Rigidbody2D& rb2)
	{
		// Box 1 left and box 2 right collision response
		if (box1.collision_flag & COLLISION_LEFT && box2.collision_flag & COLLISION_RIGHT)
		{
			// To simulate moving between rooms
			if (box1.is_trigger && box2.is_player)
			{
				trans2.position.x = trans1.scale.x + trans2.scale.x * 0.5f;
				return;
			}
			if (box2.is_trigger && box1.is_player)
			{
				trans1.position.x = trans2.scale.x + trans1.scale.x * 0.5f;
				return;
			}

			// To simulate wall collision
			if (!rb1.is_kinematic)
				trans1.position.x = box2.max.x + trans1.scale.x * 0.5f + MIN_OFFSET; // Move box1 to the right
			if (!rb2.is_kinematic)
				trans2.position.x = box1.min.x - trans2.scale.x * 0.5f - MIN_OFFSET; // Move box2 to the left

			// To simulate bounce effect
			if (!box1.is_player)
				rb1.velocity.x = -rb1.velocity.x;
			if (!box2.is_player)
				rb2.velocity.x = -rb2.velocity.x;
		}

		// Box 1 right and box 2 left collision response
		if (box1.collision_flag & COLLISION_RIGHT && box2.collision_flag & COLLISION_LEFT)
		{
			// To simulate moving between rooms
			if (box1.is_trigger && box2.is_player)
			{
				trans2.position.x = screen_width - trans1.scale.x - trans2.scale.x * 0.5f;
				return;
			}
			if (box2.is_trigger && box1.is_player)
			{
				trans1.position.x = screen_width - trans2.scale.x - trans1.scale.x * 0.5f;
				return;
			}

			// To simulate wall collision
			if (!rb1.is_kinematic)
				trans1.position.x = box2.min.x - trans1.scale.x * 0.5f - MIN_OFFSET; // Move box1 to the left
			if (!rb2.is_kinematic)
				trans2.position.x = box1.max.x + trans2.scale.x * 0.5f + MIN_OFFSET; // Move box2 to the right

			// To simulate bounce effect
			if (!box1.is_player)
				rb1.velocity.x = -rb1.velocity.x;
			if (!box2.is_player)
				rb2.velocity.x = -rb2.velocity.x;
		}

		// Box 1 top and box 2 bottom collision response
		if (box1.collision_flag & COLLISION_TOP && box2.collision_flag & COLLISION_BOTTOM)
		{
			// To simulate moving between rooms
			if (box1.is_trigger && box2.is_player)
			{
				trans2.position.y = trans1.scale.y + trans2.scale.y * 0.5f;
				return;
			}
			if (box2.is_trigger && box1.is_player)
			{
				trans1.position.y = trans2.scale.y + trans1.scale.y * 0.5f;
				return;
			}

			// To simulate wall collision
			if (!rb1.is_kinematic)
				trans1.position.y = box2.max.y + trans1.scale.y * 0.5f + MIN_OFFSET; // Move box1 to the bottom
			if (!rb2.is_kinematic)
				trans2.position.y = box1.min.y - trans2.scale.y * 0.5f - MIN_OFFSET; // Move box2 to the top

			// To simulate bounce effect
			if (!box1.is_player)
				rb1.velocity.y = -rb1.velocity.y;
			if (!box2.is_player)
				rb2.velocity.y = -rb2.velocity.y;
		}

		// Box 1 bottom and box 2 top collision response
		if (box1.collision_flag & COLLISION_BOTTOM && box2.collision_flag & COLLISION_TOP)
		{
			// To simulate moving between rooms
			if (box1.is_trigger && box2.is_player)
			{
				trans2.position.y = screen_height - trans1.scale.y - trans2.scale.y * 0.5f;
				return;
			}
			if (box2.is_trigger && box1.is_player)
			{
				trans1.position.y = screen_height - trans2.scale.y - trans1.scale.y * 0.5f;
				return;
			}

			// To simulate wall collision
			if (!rb1.is_kinematic)
				trans1.position.y = box2.min.y - trans1.scale.y * 0.5f - MIN_OFFSET; // Move box1 to the top
			if (!rb2.is_kinematic)
				trans2.position.y = box1.max.y + trans2.scale.y * 0.5f + MIN_OFFSET; // Move box2 to the bottom

			// To simulate bounce effect
			if (!box1.is_player)
				rb1.velocity.y = -rb1.velocity.y;
			if (!box2.is_player)
				rb2.velocity.y = -rb2.velocity.y;
		}
	}

	/*!***********************************************************************
	\brief
	 Implementation of collision detection between a box and the screen boundaries.
	\param[in/out] box
	 The box to collide.
	\return
	 True if the box and screen boundary intersect, false otherwise.
	*************************************************************************/
	int Collision::CollisionIntersection_BoxScreen(BoxCollider2D& box)
	{
		// might require +-camera delta if the camera is movable
		//if (box.min.x < -cameraDelta.x)

		int flag = 0;

		// Check left screen boundary
		if (box.min.x < 0)
			flag |= COLLISION_LEFT;

		// Check right screen boundary
		if (box.max.x > screen_width)
			flag |= COLLISION_RIGHT;

		// Check top screen boundary
		if (box.min.y < 0)
			flag |= COLLISION_TOP;

		// Check bottom screen boundary
		if (box.max.y > screen_height)
			flag |= COLLISION_BOTTOM;

		// No collision with any boundary
		return box.collision_flag = flag;
	}

	/*!***********************************************************************
	\brief
	 Collision response for collision between a box and the screen boundaries.
	\param[in] box
	 The box that collided.
	\param[out] trans
	 The transform component attached to the collided object.
	\param[out] rb
	 The rigidbody component attached to the collided object.
	*************************************************************************/
	void Collision::CollisionResponse_BoxScreen(const BoxCollider2D& box, Transform& trans, Rigidbody2D& rb)
	{
		// Left collision response
		if (box.collision_flag & COLLISION_LEFT)
		{
			trans.position.x = trans.scale.x * 0.5f;

			if (!box.is_player)
				rb.velocity.x = -rb.velocity.x;
		}

		// Right collision response
		if (box.collision_flag & COLLISION_RIGHT)
		{
			trans.position.x = screen_width - trans.scale.x * 0.5f;

			if (!box.is_player)
				rb.velocity.x = -rb.velocity.x;
		}

		// Top collision response
		if (box.collision_flag & COLLISION_TOP)
		{
			trans.position.y = trans.scale.y * 0.5f;

			if (!box.is_player)
				rb.velocity.y = -rb.velocity.y;
		}

		// Bottom collision response
		if (box.collision_flag & COLLISION_BOTTOM)
		{
			trans.position.y = screen_height - trans.scale.y * 0.5f;

			if (!box.is_player)
				rb.velocity.y = -rb.velocity.y;
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
}