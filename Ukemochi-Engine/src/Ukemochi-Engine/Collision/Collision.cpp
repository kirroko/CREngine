/* Start Header
*****************************************************************/
/*!
\file       Collision.cpp
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Oct 19, 2024
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
#include "../Audio/Audio.h"		  // for Audio sound effects
#include "../Application.h"		  // for screen size

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
	}

	/*!***********************************************************************
	\brief
	 Check and resolve the collisions of all the entities.
	*************************************************************************/
	void Collision::CheckCollisions()
	{
		for (auto const& entity : m_Entities)
		{
			// Get references of the first entity components
			auto& box = ECS::GetInstance().GetComponent<BoxCollider2D>(entity);
			auto& trans = ECS::GetInstance().GetComponent<Transform>(entity);
			auto& rb = ECS::GetInstance().GetComponent<Rigidbody2D>(entity);

			// Update the bounding box size
			UpdateBoundingBox(box, trans);

			for (auto const& entity2 : m_Entities)
			{
				// Skip self collision
				if (entity == entity2)
					continue;

				// Get references of the second entity components
				auto& box2 = ECS::GetInstance().GetComponent<BoxCollider2D>(entity2);
				auto& trans2 = ECS::GetInstance().GetComponent<Transform>(entity2);
				auto& rb2 = ECS::GetInstance().GetComponent<Rigidbody2D>(entity2);

				// Check collision between entities
				float tLast;
				if (BoxBox_Intersection(box, rb.velocity, box2, rb2.velocity, tLast))
					BoxBox_Response(box, box2, trans, trans2, rb, rb2);
			}

			// Check collision between entities and the screen boundaries
			if (BoxScreen_Intersection(box))
				BoxScreen_Response(box, trans, rb);
		}
	}

	/*!***********************************************************************
	\brief
	 Update the bounding box of the entity.
	*************************************************************************/
	void Collision::UpdateBoundingBox(BoxCollider2D& box, const Transform& trans)
	{
		box.min = { -BOUNDING_BOX_SIZE * trans.scale.x + trans.position.x,
					-BOUNDING_BOX_SIZE * trans.scale.y + trans.position.y };
		box.max = { BOUNDING_BOX_SIZE * trans.scale.x + trans.position.x,
					BOUNDING_BOX_SIZE * trans.scale.y + trans.position.y };
	}

	/*!***********************************************************************
	\brief
	 Implementation of collision detection between two boxes.
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
			float tLast = static_cast<float>(Ukemochi::g_FrameRateController.GetDeltaTime());
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
	 Implementation of collision detection between a box and the screen boundaries.
	*************************************************************************/
	int Collision::BoxScreen_Intersection(BoxCollider2D& box)
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
	 Implementation of collision detection between two circles.
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
	 Implementation of collision detection between a circle and a box.
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
	 Collision response for collision between two boxes.
	*************************************************************************/
	void Collision::BoxBox_Response(const BoxCollider2D& box1, const BoxCollider2D& box2, Transform& trans1, Transform& trans2, Rigidbody2D& rb1, Rigidbody2D& rb2)
	{
		if (box1.tag == "Player")
		{
			if (box2.is_trigger) // PLAYER AND TRIGGERS (trigger objects)
			{
				// PLAYER AND DOORS
				if (box2.tag == "Left Door" || box2.tag == "Right Door" || box2.tag == "Top Door" || box2.tag == "Btm Door")
					PlayerDoor_Response(box2, trans1, trans2);

				// PLAYER AND OTHER TRIGGERS (coins, checkpoints, etc..)
			}
			else // PLAYER AND STATIC/DYNAMIC (static objects / dynamic objects)
			{
				// PLAYER AND WALL (static objects)
				Static_Response(box1, box2, trans1, trans2, rb1, rb2);

				if (box2.tag == "Enemy") // PLAYER AND ENEMY (dynamic objects)
					PlayerEnemy_Response(box1, box2, rb2);
			}
		}
		else if (box1.tag == "Enemy")
		{
			// ENEMY AND WALL/TRIGGER/ENEMY (static objects / trigger objects / dynamic objects)
			Static_Response(box1, box2, trans1, trans2, rb1, rb2);
			EnemyBox_Response(box1, box2, rb1, rb2);
		}

		// Play a sound effect on collision
		if (!Audio::GetInstance().IsPlaying(HIT))
			Audio::GetInstance().PlaySoundInGroup(AudioList::HIT, ChannelGroups::LEVEL1);
	}

	/*!***********************************************************************
	\brief
	 Collision response for collision between the player and a door.
	*************************************************************************/
	void Collision::PlayerDoor_Response(const BoxCollider2D& door_box, Transform& player_trans, const Transform& door_trans)
	{
		// To simulate moving between rooms
		if (door_box.tag == "Left Door")
		{
			// If the player is colliding with the top or btm of the door, act as a wall
			if (door_box.collision_flag & COLLISION_TOP)
			{
				player_trans.position.y = door_box.min.y - player_trans.scale.y * 0.5f - MIN_OFFSET;
			}
			else if (door_box.collision_flag & COLLISION_BOTTOM)
			{
				player_trans.position.y = door_box.max.y + player_trans.scale.y * 0.5f + MIN_OFFSET;
			}
			else // Player entered the left door
			{
				player_trans.position.x = screen_width - door_trans.scale.x - player_trans.scale.x * 0.5f;
			}
		}
		else if (door_box.tag == "Right Door")
		{
			// If the player is colliding with the top or btm of the door, act as a wall
			if (door_box.collision_flag & COLLISION_TOP)
			{
				player_trans.position.y = door_box.min.y - player_trans.scale.y * 0.5f - MIN_OFFSET;
			}
			else if (door_box.collision_flag & COLLISION_BOTTOM)
			{
				player_trans.position.y = door_box.max.y + player_trans.scale.y * 0.5f + MIN_OFFSET;
			}
			else // Player entered the right door
			{
				player_trans.position.x = door_trans.scale.x + player_trans.scale.x * 0.5f;
			}
		}
		else if (door_box.tag == "Top Door")
		{
			// If the player is colliding with the left or right of the door, act as a wall
			if (door_box.collision_flag & COLLISION_LEFT)
			{
				player_trans.position.x = door_box.min.x - player_trans.scale.x * 0.5f - MIN_OFFSET;
			}
			else if (door_box.collision_flag & COLLISION_RIGHT)
			{
				player_trans.position.x = door_box.max.x + player_trans.scale.x * 0.5f + MIN_OFFSET;
			}
			else // Player entered the top door
			{
				player_trans.position.y = screen_height - door_trans.scale.y - player_trans.scale.y * 0.5f;
			}
		}
		else if (door_box.tag == "Btm Door")
		{
			// If the player is colliding with the left or right of the door, act as a wall
			if (door_box.collision_flag & COLLISION_LEFT)
			{
				player_trans.position.x = door_box.min.x - player_trans.scale.x * 0.5f - MIN_OFFSET;
			}
			else if (door_box.collision_flag & COLLISION_RIGHT)
			{
				player_trans.position.x = door_box.max.x + player_trans.scale.x * 0.5f + MIN_OFFSET;
			}
			else // Player entered the bottom door
			{
				player_trans.position.y = door_trans.scale.y + player_trans.scale.y * 0.5f;
			}
		}
	}

	/*!***********************************************************************
	\brief
	 Collision response for static collision between two boxes.
	*************************************************************************/
	void Collision::Static_Response(const BoxCollider2D& box1, const BoxCollider2D& box2, Transform& trans1, Transform& trans2, const Rigidbody2D& rb1, const Rigidbody2D& rb2)
	{
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
	 Collision response for collision between the player and a enemy.
	*************************************************************************/
	void Collision::PlayerEnemy_Response(const BoxCollider2D& player_box, const BoxCollider2D& enemy_box, Rigidbody2D& enemy_rb)
	{
		// Player left and enemy right collision response
		if (player_box.collision_flag & COLLISION_LEFT && enemy_box.collision_flag & COLLISION_RIGHT)
		{
			// To simulate bounce effect
			enemy_rb.velocity.x = -enemy_rb.velocity.x;
		}

		// Player right and enemy left collision response
		if (player_box.collision_flag & COLLISION_RIGHT && enemy_box.collision_flag & COLLISION_LEFT)
		{
			// To simulate bounce effect
			enemy_rb.velocity.x = -enemy_rb.velocity.x;
		}

		// Player top and enemy bottom collision response
		if (player_box.collision_flag & COLLISION_TOP && enemy_box.collision_flag & COLLISION_BOTTOM)
		{
			// To simulate bounce effect
			enemy_rb.velocity.y = -enemy_rb.velocity.y;
		}

		// Box 1 bottom and box 2 top collision response
		if (player_box.collision_flag & COLLISION_BOTTOM && enemy_box.collision_flag & COLLISION_TOP)
		{
			// To simulate bounce effect
			enemy_rb.velocity.y = -enemy_rb.velocity.y;
		}
	}

	/*!***********************************************************************
	\brief
	 Collision response for collision between a enemy and a box.
	*************************************************************************/
	void Collision::EnemyBox_Response(const BoxCollider2D& box1, const BoxCollider2D& box2, Rigidbody2D& rb1, Rigidbody2D& rb2)
	{
		// Box 1 left and box 2 right collision response
		if (box1.collision_flag & COLLISION_LEFT && box2.collision_flag & COLLISION_RIGHT)
		{
			// To simulate bounce effect
			if (box1.tag == "Enemy")
				rb1.velocity.x = -rb1.velocity.x;
			if (box2.tag == "Enemy")
				rb2.velocity.x = -rb2.velocity.x;
		}

		// Box 1 right and box 2 left collision response
		if (box1.collision_flag & COLLISION_RIGHT && box2.collision_flag & COLLISION_LEFT)
		{
			// To simulate bounce effect
			if (box1.tag == "Enemy")
				rb1.velocity.x = -rb1.velocity.x;
			if (box2.tag == "Enemy")
				rb2.velocity.x = -rb2.velocity.x;
		}

		// Box 1 top and box 2 bottom collision response
		if (box1.collision_flag & COLLISION_TOP && box2.collision_flag & COLLISION_BOTTOM)
		{
			// To simulate bounce effect
			if (box1.tag == "Enemy")
				rb1.velocity.y = -rb1.velocity.y;
			if (box2.tag == "Enemy")
				rb2.velocity.y = -rb2.velocity.y;
		}

		// Box 1 bottom and box 2 top collision response
		if (box1.collision_flag & COLLISION_BOTTOM && box2.collision_flag & COLLISION_TOP)
		{
			// To simulate bounce effect
			if (box1.tag == "Enemy")
				rb1.velocity.y = -rb1.velocity.y;
			if (box2.tag == "Enemy")
				rb2.velocity.y = -rb2.velocity.y;
		}
	}

	/*!***********************************************************************
	\brief
	 Collision response for collision between a box and the screen boundaries.
	*************************************************************************/
	void Collision::BoxScreen_Response(const BoxCollider2D& box, Transform& trans, Rigidbody2D& rb)
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