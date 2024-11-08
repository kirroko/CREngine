/* Start Header
*****************************************************************/
/*!
\file       Collision.cpp
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Nov 6, 2024
\brief      This file contains the definition of the Collision system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "PreCompile.h"
#include "Collision.h"			// for forward declaration
#include "../Math/MathUtils.h"	// for min, max, abs
#include "../FrameController.h"	// for GetFixedDeltaTime
#include "../Audio/Audio.h"		// for Audio sound effects
#include "../Application.h"		// for screen size

namespace Ukemochi
{
	/*!***********************************************************************
	\brief
	 Initialize the collision system.
	*************************************************************************/
	void Collision::Init()
	{
		// Get the screen width and height
		Application& app = Application::Get();
		screen_width = app.GetWindow().GetWidth();
		screen_height = app.GetWindow().GetHeight();

		ConvexCollider2D convex1;
		convex1.vertices.push_back(Vec2{ 0,0 });
		convex1.vertices.push_back(Vec2{ 1,0 });
		convex1.vertices.push_back(Vec2{ 1,1 });
		convex1.vertices.push_back(Vec2{ 0,1 });

		ConvexCollider2D convex2;
		convex2.vertices.push_back(Vec2{ 0.5f,0.5f });
		convex2.vertices.push_back(Vec2{ 1.5f,0.5f });
		convex2.vertices.push_back(Vec2{ 1.5f,1.5f });
		convex2.vertices.push_back(Vec2{ 0.5f,1.5f });

		if (ConvexConvex_Intersection(convex1, convex2))
			std::cout << "Convex collided\n";
	}

	/*!***********************************************************************
	\brief
	 Check the collisions of all the objects.
	*************************************************************************/
	void Collision::CheckCollisions()
	{
		// Update the collision based on the number of steps
		for (int step = 0; step < g_FrameRateController.GetCurrentNumberOfSteps(); ++step)
		{
			for (auto const& entity : m_Entities)
			{
				// Get references of the first entity components
				auto& trans1 = ECS::GetInstance().GetComponent<Transform>(entity);
				auto& box1 = ECS::GetInstance().GetComponent<BoxCollider2D>(entity);
				auto& rb1 = ECS::GetInstance().GetComponent<Rigidbody2D>(entity);

				//auto& convex1 = ECS::GetInstance().GetComponent<ConvexCollider2D>(entity);

				// Update the bounding box size
				UpdateBoundingBox(box1, trans1);

				for (auto const& entity2 : m_Entities)
				{
					// Skip self collision
					if (entity == entity2)
						continue;

					// Get references of the second entity components
					auto& trans2 = ECS::GetInstance().GetComponent<Transform>(entity2);
					auto& box2 = ECS::GetInstance().GetComponent<BoxCollider2D>(entity2);
					auto& rb2 = ECS::GetInstance().GetComponent<Rigidbody2D>(entity2);

					//auto& convex2 = ECS::GetInstance().GetComponent<ConvexCollider2D>(entity);

					// Check collision between two box objects
					float tLast{};
					if (BoxBox_Intersection(box1, rb1.velocity, box2, rb2.velocity, tLast))
						BoxBox_Response(trans1, box1, rb1, trans2, box2, rb2, tLast);

					// Check collision between two convex objects
					//if (ConvexConvex_Intersection(convex1, convex2))
					//	std::cout << "Convex collided\n";
				}

				// Check collision between box objects and the screen boundaries
				if (BoxScreen_Intersection(box1))
					BoxScreen_Response(trans1, box1, rb1);
			}
		}
	}

	/*!***********************************************************************
	\brief
	 Update the bounding box of the object.
	*************************************************************************/
	void Collision::UpdateBoundingBox(BoxCollider2D& box, const Transform& trans)
	{
		box.min = { -BOUNDING_BOX_SIZE * trans.scale.x + trans.position.x,
					-BOUNDING_BOX_SIZE * trans.scale.y + trans.position.y };
		box.max = { BOUNDING_BOX_SIZE * trans.scale.x + trans.position.x,
					BOUNDING_BOX_SIZE * trans.scale.y + trans.position.y };

		//// Define the corners of the box
		//Vec2 corners[4] = {
		//	{-BOUNDING_BOX_SIZE * trans.scale.x, -BOUNDING_BOX_SIZE * trans.scale.y}, // bottom-left
		//	{ BOUNDING_BOX_SIZE * trans.scale.x, -BOUNDING_BOX_SIZE * trans.scale.y}, // bottom-right
		//	{ BOUNDING_BOX_SIZE * trans.scale.x,  BOUNDING_BOX_SIZE * trans.scale.y}, // top-right
		//	{-BOUNDING_BOX_SIZE * trans.scale.x,  BOUNDING_BOX_SIZE * trans.scale.y}  // top-left
		//};

		//// Rotate all corners and translate by position
		//for (int i = 0; i < 4; ++i)
		//{
		//	Vec2 rotated = Vec2Rotate(corners[i], trans.rotation);
		//	corners[i].x = rotated.x + trans.position.x;
		//	corners[i].y = rotated.y + trans.position.y;
		//}

		//// Find min and max points
		//box.min.x = std::min({ corners[0].x, corners[1].x, corners[2].x, corners[3].x });
		//box.min.y = std::min({ corners[0].y, corners[1].y, corners[2].y, corners[3].y });

		//box.max.x = std::max({ corners[0].x, corners[1].x, corners[2].x, corners[3].x });
		//box.max.y = std::max({ corners[0].y, corners[1].y, corners[2].y, corners[3].y });
	}

	/*!***********************************************************************
	\brief
	 Collision detection between two boxes.
	*************************************************************************/
	bool Collision::BoxBox_Intersection(BoxCollider2D& box1, const Vec2& vel1, BoxCollider2D& box2, const Vec2& vel2, float& firstTimeOfCollision)
	{
		// Create collision flags for both boxes
		int box1_flag = 0;
		int box2_flag = 0;

		// Check for static collision detection between boxes
		if (box1.max.x < box2.min.x || box1.min.x > box2.max.x ||
			box1.max.y < box2.min.y || box1.min.y > box2.max.y)
		{
			// No overlapping boxes
			// return 0;

			// Initialize and calculate the new velocity of vRel
			firstTimeOfCollision = 0.0f;
			float tLast = static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
			Vec2 vRel = { vel2.x - vel1.x, vel2.y - vel1.y };

			// Check for collision in the x-axis
			if (vRel.x < 0) // Moving left
			{
				if (box1.min.x > box2.max.x)
					return 0; // No intersection and is moving away

				// Calculate time of first collision
				if (box1.max.x < box2.min.x)
				{
					firstTimeOfCollision = max((box1.max.x - box2.min.x) / vRel.x, firstTimeOfCollision);

					box1_flag |= COLLISION_RIGHT;
					box2_flag |= COLLISION_LEFT;
				}
				if (box1.min.x < box2.max.x)
					tLast = min((box1.min.x - box2.max.x) / vRel.x, tLast);
			}
			else if (vRel.x > 0) // Moving right
			{
				if (box1.max.x < box2.min.x)
					return 0; // No intersection and is moving away

				// Calculate time of first collision
				if (box1.min.x > box2.max.x)
				{
					firstTimeOfCollision = max((box1.min.x - box2.max.x) / vRel.x, firstTimeOfCollision);

					box1_flag |= COLLISION_LEFT;
					box2_flag |= COLLISION_RIGHT;
				}
				if (box1.max.x > box2.min.x)
					tLast = min((box1.max.x - box2.min.x) / vRel.x, tLast);
			}
			else if (vRel.x == 0)
			{
				if (box1.max.x < box2.min.x || box1.min.x > box2.max.x)
					return 0; // Moving parallel to the opposite coordinate axis
			}

			if (firstTimeOfCollision > tLast)
				return 0; // No intersection

			// Check for collision in the y-axis
			if (vRel.y < 0) // Moving down
			{
				if (box1.min.y > box2.max.y)
					return 0; // No intersection and is moving away

				// Calculate time of first collision
				if (box1.max.y < box2.min.y)
				{
					firstTimeOfCollision = max((box1.max.y - box2.min.y) / vRel.y, firstTimeOfCollision);

					box1_flag |= COLLISION_BOTTOM;
					box2_flag |= COLLISION_TOP;
				}
				if (box1.min.y < box2.max.y)
					tLast = min((box1.min.y - box2.max.y) / vRel.y, tLast);
			}
			else if (vRel.y > 0) // Moving up
			{
				if (box1.max.y < box2.min.y)
					return 0; // No intersection and is moving away

				if (box1.min.y > box2.max.y)
				{
					firstTimeOfCollision = max((box1.min.y - box2.max.y) / vRel.y, firstTimeOfCollision);

					box1_flag |= COLLISION_TOP;
					box2_flag |= COLLISION_BOTTOM;
				}
				if (box1.max.y > box2.min.y)
					tLast = min((box1.max.y - box2.min.y) / vRel.y, tLast);
			}
			else if (vRel.y == 0)
			{
				if (box1.max.y < box2.min.y || box1.min.y > box2.max.y)
					return 0; // Moving parallel to the opposite coordinate axis
			}

			if (firstTimeOfCollision > tLast)
				return 0; // No intersection

			// Set the collision flags to the boxes
			box1.collision_flag = box1_flag;
			box2.collision_flag = box2_flag;

			// Intersection detected
			return 1;
		}
		else
		{
			// Calculate overlaps on each axis
			float left_overlap = box2.max.x - box1.min.x;  // Distance from box1's left to box2's right
			float right_overlap = box1.max.x - box2.min.x; // Distance from box1's right to box2's left
			float top_overlap = box1.max.y - box2.min.y;   // Distance from box1's top to box2's bottom
			float btm_overlap = box2.max.y - box1.min.y;   // Distance from box1's bottom to box2's top

			// Determine which side has the least overlap (smallest distance)
			if (left_overlap < right_overlap && left_overlap < top_overlap && left_overlap < btm_overlap)
			{
				box1_flag |= COLLISION_LEFT;
				box2_flag |= COLLISION_RIGHT;
			}
			else if (right_overlap < left_overlap && right_overlap < top_overlap && right_overlap < btm_overlap)
			{
				box1_flag |= COLLISION_RIGHT;
				box2_flag |= COLLISION_LEFT;
			}
			else if (top_overlap < left_overlap && top_overlap < right_overlap && top_overlap < btm_overlap)
			{
				box1_flag |= COLLISION_BOTTOM;
				box2_flag |= COLLISION_TOP;
			}
			else
			{
				box1_flag |= COLLISION_TOP;
				box2_flag |= COLLISION_BOTTOM;
			}

			// Set the collision flags to the boxes
			box1.collision_flag = box1_flag;
			box2.collision_flag = box2_flag;

			// Intersection detected
			return 1;
		}
	}

	/*!***********************************************************************
	\brief
	 Collision detection between a box and the screen boundaries.
	*************************************************************************/
	int Collision::BoxScreen_Intersection(BoxCollider2D& box)
	{
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
	 Collision detection between two circles.
	*************************************************************************/
	bool Collision::CircleCircle_Intersection(const CircleCollider2D& circle1, const CircleCollider2D& circle2)
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
	 Collision detection between a circle and a box.
	*************************************************************************/
	bool Collision::CircleBox_Intersection(const CircleCollider2D& circle, const BoxCollider2D& box)
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
	 Check for collision detection between two convex.
	*************************************************************************/
	bool Collision::ConvexConvex_Intersection(const ConvexCollider2D& convex1, const ConvexCollider2D& convex2)
	{
		// Test each edge of the first convex
		size_t count = convex1.vertices.size();
		for (size_t i = 0; i < count; i++)
		{
			Vec2 edge = convex1.vertices[(i + 1) % count] - convex1.vertices[i];
			Vec2 axis = edge.perpendicular();

			float min0, max0, min1, max1;
			ComputeProjInterval(convex1, axis, min0, max0);
			ComputeProjInterval(convex2, axis, min1, max1);

			// Check if the projections overlap
			if (max0 < min1 || max1 < min0)
				return false; // No collision
		}

		// Test each edge of the second convex
		count = convex2.vertices.size();
		for (size_t i = 0; i < count; i++)
		{
			Vec2 edge = convex2.vertices[(i + 1) % count] - convex2.vertices[i];
			Vec2 axis = edge.perpendicular();

			float min0, max0, min1, max1;
			ComputeProjInterval(convex1, axis, min0, max0);
			ComputeProjInterval(convex2, axis, min1, max1);

			// Check if the projections overlap
			if (max0 < min1 || max1 < min0)
				return false; // No collision
		}

		return true; // Collision detected
	}

	/*!***********************************************************************
	\brief
	 Project the vertices of a convex onto a given axis and find the min and max projection values.
	*************************************************************************/
	void Collision::ComputeProjInterval(const ConvexCollider2D& convex, const Vec2& axis, float& min, float& max)
	{
		min = max = Vec2DotProduct(convex.vertices[0], axis);

		for (size_t i = 1; i < convex.vertices.size(); i++)
		{
			float projection = Vec2DotProduct(convex.vertices[i], axis);
			if (projection < min)
				min = projection;
			else if (projection > max)
				max = projection;
		}
	}

	/*!***********************************************************************
	\brief
	 Collision response between two objects.
	*************************************************************************/
	void Collision::BoxBox_Response(Transform& trans1, BoxCollider2D& box1, Rigidbody2D& rb1, Transform& trans2, BoxCollider2D& box2, Rigidbody2D& rb2, float firstTimeOfCollision)
	{
		// PLAYER AND TRIGGER
		if (box1.tag == "Player" && box2.is_trigger)
			Trigger_Response(trans1, trans2, box2);
		else // STATIC AND DYNAMIC / DYNAMIC AND DYNAMIC
		{
			Static_Response(trans1, box1, rb1, trans2, box2, rb2);
			StaticDynamic_Response(trans1, box1, rb1, trans2, box2, rb2, firstTimeOfCollision);
		}

		// Play a sound effect on collision
		if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsPlaying(HIT))
			ECS::GetInstance().GetSystem<Audio>()->GetInstance().PlaySoundInGroup(AudioList::HIT, ChannelGroups::LEVEL1);
	}

	/*!***********************************************************************
	\brief
	 Collision response between a static object and a dynamic object, and between two dynamic objects.
	*************************************************************************/
	void Collision::StaticDynamic_Response(Transform& trans1, BoxCollider2D& box1, Rigidbody2D& rb1, Transform& trans2, BoxCollider2D& box2, Rigidbody2D& rb2, float firstTimeOfCollision)
	{
		// Skip trigger objects
		if (box1.tag == "Player" && box2.is_trigger || box2.tag == "Player" && box1.is_trigger)
			return;

		// Move objects to the point of collision
		if (!rb1.is_kinematic)
			trans1.position += rb1.velocity * firstTimeOfCollision * static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
		if (!rb2.is_kinematic)
			trans2.position += rb2.velocity * firstTimeOfCollision * static_cast<float>(g_FrameRateController.GetFixedDeltaTime());

		// Check collision flags and adjust velocities based on impact direction
		if (box1.collision_flag & COLLISION_RIGHT && box2.collision_flag & COLLISION_LEFT)
		{
			// Collision on the right side of box1 and left side of box2
			rb1.velocity.x = -abs(rb1.velocity.x); // Reverse box1's x velocity
			rb2.velocity.x = abs(rb2.velocity.x); // Reverse box2's x velocity
		}
		else if (box1.collision_flag & COLLISION_LEFT && box2.collision_flag & COLLISION_RIGHT)
		{
			// Collision on the left side of box1 and right side of box2
			rb1.velocity.x = abs(rb1.velocity.x); // Reverse box1's x velocity
			rb2.velocity.x = -abs(rb2.velocity.x); // Reverse box2's x velocity
		}

		if (box1.collision_flag & COLLISION_TOP && box2.collision_flag & COLLISION_BOTTOM)
		{
			// Collision on the top side of box1 and bottom side of box2
			rb1.velocity.y = abs(rb1.velocity.y); // Reverse box1's y velocity
			rb2.velocity.y = -abs(rb2.velocity.y); // Reverse box2's y velocity
		}
		else if (box1.collision_flag & COLLISION_BOTTOM && box2.collision_flag & COLLISION_TOP)
		{
			// Collision on the bottom side of box1 and top side of box2
			rb1.velocity.y = -abs(rb1.velocity.y); // Reverse box1's y velocity
			rb2.velocity.y = abs(rb2.velocity.y); // Reverse box2's y velocity
		}

		// Reset collision flags after handling response
		box1.collision_flag = 0;
		box2.collision_flag = 0;
	}

	/*!***********************************************************************
	\brief
	 Collision response between a static object and a dynamic object.
	*************************************************************************/
	void Collision::Static_Response(Transform& trans1, const BoxCollider2D& box1, const Rigidbody2D& rb1, Transform& trans2, const BoxCollider2D& box2, const Rigidbody2D& rb2)
	{
		// STATIC AND DYNAMIC
		// Box 1 left and box 2 right collision response
		if (box1.collision_flag & COLLISION_LEFT && box2.collision_flag & COLLISION_RIGHT)
		{
			// To simulate wall collision
			if (!rb1.is_kinematic)
				trans1.position.x = box2.max.x + trans1.scale.x * 0.5f + MIN_OFFSET; // Move box1 to the right
			if (!rb2.is_kinematic)
				trans2.position.x = box1.min.x - trans2.scale.x * 0.5f - MIN_OFFSET; // Move box2 to the left
		}

		// Box 1 right and box 2 left collision response
		if (box1.collision_flag & COLLISION_RIGHT && box2.collision_flag & COLLISION_LEFT)
		{
			// To simulate wall collision
			if (!rb1.is_kinematic)
				trans1.position.x = box2.min.x - trans1.scale.x * 0.5f - MIN_OFFSET; // Move box1 to the left
			if (!rb2.is_kinematic)
				trans2.position.x = box1.max.x + trans2.scale.x * 0.5f + MIN_OFFSET; // Move box2 to the right
		}

		// Box 1 top and box 2 bottom collision response
		if (box1.collision_flag & COLLISION_TOP && box2.collision_flag & COLLISION_BOTTOM)
		{
			// To simulate wall collision
			if (!rb1.is_kinematic)
				trans1.position.y = box2.max.y + trans1.scale.y * 0.5f + MIN_OFFSET; // Move box1 to the bottom
			if (!rb2.is_kinematic)
				trans2.position.y = box1.min.y - trans2.scale.y * 0.5f - MIN_OFFSET; // Move box2 to the top
		}

		// Box 1 bottom and box 2 top collision response
		if (box1.collision_flag & COLLISION_BOTTOM && box2.collision_flag & COLLISION_TOP)
		{
			// To simulate wall collision
			if (!rb1.is_kinematic)
				trans1.position.y = box2.min.y - trans1.scale.y * 0.5f - MIN_OFFSET; // Move box1 to the top
			if (!rb2.is_kinematic)
				trans2.position.y = box1.max.y + trans2.scale.y * 0.5f + MIN_OFFSET; // Move box2 to the bottom
		}
	}

	/*!***********************************************************************
	\brief
	 Collision response between two dynamic objects.
	*************************************************************************/
	void Collision::Dynamic_Response(Transform& trans1, Rigidbody2D& rb1, Transform& trans2, Rigidbody2D& rb2)
	{
		// DYNAMIC AND DYNAMIC
		// Calculate the difference in positions
		Vector2D difference = trans2.position - trans1.position;
		Vector2D combine_half_scale = (trans1.scale + trans2.scale) * 0.5f;

		// Calculate the overlap on the x and y axis
		float overlap_x = combine_half_scale.x - std::abs(difference.x);
		float overlap_y = combine_half_scale.y - std::abs(difference.y);

		// Check if there is an overlap on both axis
		if (overlap_x > 0 && overlap_y > 0)
		{
			// Determine the minimal axis of separation
			if (overlap_x < overlap_y)
			{
				// Resolve along the x-axis
				float penetration = overlap_x;
				float response_x = penetration / static_cast<float>(g_FrameRateController.GetFixedDeltaTime());

				// First object is on the left, second object is on the right
				if (difference.x > 0)
				{
					// Check if the first object has physics
					if (!rb1.is_kinematic)
					{
						trans1.position.x -= penetration * 0.5f;
						rb1.velocity.x = -response_x;
					}

					// Check if the second object has physics
					if (!rb2.is_kinematic)
					{
						trans2.position.x += penetration * 0.5f;
						rb2.velocity.x = response_x;
					}
				}
				else // First object is on the right, second object is on the left
				{
					// Check if the first object has physics
					if (!rb1.is_kinematic)
					{
						trans1.position.x += penetration * 0.5f;
						rb1.velocity.x = response_x;
					}

					// Check if the second object has physics
					if (!rb2.is_kinematic)
					{
						trans2.position.x -= penetration * 0.5f;
						rb2.velocity.x = -response_x;
					}
				}
			}
			else
			{
				// Resolve along the y-axis
				float penetration = overlap_y;
				float response_y = penetration / static_cast<float>(g_FrameRateController.GetFixedDeltaTime());

				// First object is on the bottom, second object is on the top
				if (difference.y > 0)
				{
					// Check if the first object has physics
					if (!rb1.is_kinematic)
					{
						trans1.position.y -= penetration * 0.5f;
						rb1.velocity.y = -response_y;
					}

					// Check if the second object has physics
					if (!rb2.is_kinematic)
					{
						trans2.position.y += penetration * 0.5f;
						rb2.velocity.y = response_y;
					}
				}
				else // First object is on the top, second object is on the bottom
				{
					// Check if the first object has physics
					if (!rb1.is_kinematic)
					{
						trans1.position.y += penetration * 0.5f;
						rb1.velocity.y = response_y;
					}

					// Check if the second object has physics
					if (!rb2.is_kinematic)
					{
						trans2.position.y -= penetration * 0.5f;
						rb2.velocity.y = -response_y;
					}
				}
			}
		}
	}

	/*!***********************************************************************
	\brief
	 Collision response between the player and a trigger object.
	*************************************************************************/
	void Collision::Trigger_Response(Transform& player_trans, const Transform& trigger_trans, const BoxCollider2D& trigger_box)
	{
		// PLAYER AND DOORS
		// To simulate moving between rooms
		if (trigger_box.tag == "Left Door")
		{
			// If the player is colliding with the top or btm of the door, act as a wall
			if (trigger_box.collision_flag & COLLISION_TOP)
				player_trans.position.y = trigger_box.min.y - player_trans.scale.y * 0.5f - MIN_OFFSET;
			else if (trigger_box.collision_flag & COLLISION_BOTTOM)
				player_trans.position.y = trigger_box.max.y + player_trans.scale.y * 0.5f + MIN_OFFSET;
			else // Player entered the left door
				player_trans.position.x = screen_width - trigger_trans.scale.x - player_trans.scale.x * 0.5f;
		}
		else if (trigger_box.tag == "Right Door")
		{
			// If the player is colliding with the top or btm of the door, act as a wall
			if (trigger_box.collision_flag & COLLISION_TOP)
				player_trans.position.y = trigger_box.min.y - player_trans.scale.y * 0.5f - MIN_OFFSET;
			else if (trigger_box.collision_flag & COLLISION_BOTTOM)
				player_trans.position.y = trigger_box.max.y + player_trans.scale.y * 0.5f + MIN_OFFSET;
			else // Player entered the right door
				player_trans.position.x = trigger_trans.scale.x + player_trans.scale.x * 0.5f;
		}
		else if (trigger_box.tag == "Top Door")
		{
			// If the player is colliding with the left or right of the door, act as a wall
			if (trigger_box.collision_flag & COLLISION_LEFT)
				player_trans.position.x = trigger_box.min.x - player_trans.scale.x * 0.5f - MIN_OFFSET;
			else if (trigger_box.collision_flag & COLLISION_RIGHT)
				player_trans.position.x = trigger_box.max.x + player_trans.scale.x * 0.5f + MIN_OFFSET;
			else // Player entered the top door
				player_trans.position.y = screen_height - trigger_trans.scale.y - player_trans.scale.y * 0.5f;
		}
		else if (trigger_box.tag == "Btm Door")
		{
			// If the player is colliding with the left or right of the door, act as a wall
			if (trigger_box.collision_flag & COLLISION_LEFT)
				player_trans.position.x = trigger_box.min.x - player_trans.scale.x * 0.5f - MIN_OFFSET;
			else if (trigger_box.collision_flag & COLLISION_RIGHT)
				player_trans.position.x = trigger_box.max.x + player_trans.scale.x * 0.5f + MIN_OFFSET;
			else // Player entered the bottom door
				player_trans.position.y = trigger_trans.scale.y + player_trans.scale.y * 0.5f;
		}

		// PLAYER AND OTHER TRIGGERS (coins, checkpoints, etc..)
	}

	/*!***********************************************************************
	\brief
	 Collision response between an object and the screen boundaries.
	*************************************************************************/
	void Collision::BoxScreen_Response(Transform& trans, const BoxCollider2D& box, Rigidbody2D& rb)
	{
		// Colliding with left screen boundary
		if (box.collision_flag & COLLISION_LEFT)
		{
			trans.position.x = trans.scale.x * 0.5f;

			if (box.tag == "Enemy")
				rb.velocity.x = -rb.velocity.x;
		}

		// Colliding with right screen boundary
		if (box.collision_flag & COLLISION_RIGHT)
		{
			trans.position.x = screen_width - trans.scale.x * 0.5f;

			if (box.tag == "Enemy")
				rb.velocity.x = -rb.velocity.x;
		}

		// Colliding with top screen boundary
		if (box.collision_flag & COLLISION_TOP)
		{
			trans.position.y = trans.scale.y * 0.5f;

			if (box.tag == "Enemy")
				rb.velocity.y = -rb.velocity.y;
		}

		// Colliding with bottom screen boundary
		if (box.collision_flag & COLLISION_BOTTOM)
		{
			trans.position.y = screen_height - trans.scale.y * 0.5f;

			if (box.tag == "Enemy")
				rb.velocity.y = -rb.velocity.y;
		}
	}
}