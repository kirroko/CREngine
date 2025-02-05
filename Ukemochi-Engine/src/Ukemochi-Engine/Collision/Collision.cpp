/* Start Header ************************************************************************/
/*!
\file       Collision.cpp
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Jan 19, 2025
\brief      This file contains the definition of the Collision system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "Collision.h"							// for forward declaration
#include "../Math/MathUtils.h"					// for min, max, abs
#include "../FrameController.h"					// for GetFixedDeltaTime
#include "../Audio/Audio.h"						// for Audio sound effects
#include "../Application.h"						// for screen size
#include "../Factory/GameObjectManager.h"		// for game object tag
#include "Ukemochi-Engine/Logic/Scripting.h"	// for invoking OnCollisonEnter2D
#include "../Game/DungeonManager.h"				// for room size and current room ID
#include "../Physics/Physics.h"					// for knockback effect
#include "Ukemochi-Engine/Game/PlayerManager.h" // for player data
#include "../Game/EnemyManager.h"				// for enemy data

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

		// Initialize the quadtree
		BoxCollider2D bounds;
		bounds.min = Vec2{ 0, 0 };
		bounds.max = Vec2{ static_cast<float>(screen_width), static_cast<float>(screen_height) };
		quadtree = std::make_unique<QuadTree>(0, bounds);

		// Find the player entity
		player = static_cast<EntityID>(-1);
		for (auto const& entity : m_Entities)
		{
			if (GameObjectManager::GetInstance().GetGO(entity)->GetTag() == "Player")
			{
				player = entity;
				break;
			}
		}
	}

	/*!***********************************************************************
	\brief
	 Check the collisions of all the objects.
	*************************************************************************/
	void Collision::CheckCollisions()
	{
		// Update the collision based on the number of steps
		/*/for (int step = 0; step < g_FrameRateController.GetCurrentNumberOfSteps(); ++step)
		{
			// Clear the quadtree
			quadtree->Clear();

			// Update the bounding boxes of the entities and insert them into the quadtree
			for (auto const& entity : m_Entities)
			{
				// Skip if the entity is not active
				if (!GameObjectManager::GetInstance().GetGO(entity)->GetActive())
					continue;

				// Get the tag of the entity
				std::string tag = GameObjectManager::GetInstance().GetGO(entity)->GetTag();

				// Get references of the entity components
				auto& trans = ECS::GetInstance().GetComponent<Transform>(entity);
				auto& box = ECS::GetInstance().GetComponent<BoxCollider2D>(entity);

				// Update the bounding box size
				UpdateBoundingBox(box, trans, tag);

				// Insert the entity into the quadtree
				quadtree->Insert(entity, box);
			}

			// Perform broad-phase collision checks
			for (auto const& entity1 : m_Entities)
			{
				// Skip if the entity is not active
				if (!GameObjectManager::GetInstance().GetGO(entity1)->GetActive())
					continue;

				// Get references of the first entity components
				auto& box1 = ECS::GetInstance().GetComponent<BoxCollider2D>(entity1);
				auto& rb1 = ECS::GetInstance().GetComponent<Rigidbody2D>(entity1);

				// Get the vector of potential collisions within the quad
				std::vector<EntityID> potential_collisions;
				quadtree->Retrieve(potential_collisions, box1);

				for (auto const& entity2 : potential_collisions)
				{
					// Skip self collision
					if (entity1 == entity2)
						continue;

					// Get references of the second entity components
					auto& box2 = ECS::GetInstance().GetComponent<BoxCollider2D>(entity2);
					auto& rb2 = ECS::GetInstance().GetComponent<Rigidbody2D>(entity2);

					// Perform narrow-phase collision checks, check collision between two box objects
					float tLast{};
					if (BoxBox_Intersection(box1, rb1.velocity, box2, rb2.velocity, tLast))
						BoxBox_Response(entity1, entity2, tLast);
				}
			}
		}*/

		// OLD IMPLEMENTATION
		// Update the collision based on the number of steps
		for (int step = 0; step < g_FrameRateController.GetCurrentNumberOfSteps(); ++step)
		{
			for (auto const& entity1 : m_Entities)
			{
				// Skip if the first entity is not active
				if (!GameObjectManager::GetInstance().GetGO(entity1)->GetActive())
					continue;

				// Get the tag of the first entity
				std::string tag1 = GameObjectManager::GetInstance().GetGO(entity1)->GetTag();

				// Get references of the first entity components
				auto& trans1 = ECS::GetInstance().GetComponent<Transform>(entity1);
				auto& box1 = ECS::GetInstance().GetComponent<BoxCollider2D>(entity1);
				auto& rb1 = ECS::GetInstance().GetComponent<Rigidbody2D>(entity1);

				// Update the bounding box size
				UpdateBoundingBox(box1, trans1, tag1);

				for (auto const& entity2 : m_Entities)
				{
					// Skip self collision
					if (entity1 == entity2)
						continue;

					// Skip if the second entity is not active
					if (!GameObjectManager::GetInstance().GetGO(entity2)->GetActive())
						continue;

					// Get references of the second entity components
					auto& box2 = ECS::GetInstance().GetComponent<BoxCollider2D>(entity2);
					auto& rb2 = ECS::GetInstance().GetComponent<Rigidbody2D>(entity2);

					// Check collision between two box objects
					float tLast{};
					if (BoxBox_Intersection(box1, rb1.velocity, box2, rb2.velocity, tLast))
						BoxBox_Response(entity1, entity2, tLast);
				}
			}
		}//*/
	}

	/*!***********************************************************************
	\brief
	 Update the bounding box of the object.
	\param[out] box
	 The BoxCollider2D component attached to the object.
	\param[in] trans
	 The Transform component attached to the object.
	\param[in] tag
	 The tag of the object.
	*************************************************************************/
	void Collision::UpdateBoundingBox(BoxCollider2D& box, const Transform& trans, const std::string& tag)
	{
		if (tag == "Player" || tag == "Enemy" || tag == "Environment")
		{
			// Lower half of the object (legs or bottom part)
			box.min = { -BOUNDING_BOX_SIZE * trans.scale.x + trans.position.x,
						trans.position.y - BOUNDING_BOX_SIZE * trans.scale.y / 1.25f };  // Min Y is halfway down the object 1.5f
			box.max = { BOUNDING_BOX_SIZE * trans.scale.x + trans.position.x,
						trans.position.y };  // Max Y stops at the object's center
		}
		else if (tag == "Knife")
		{
			box.min = { -BOUNDING_BOX_SIZE  * (0.6f * trans.scale.x) + trans.position.x,
						-BOUNDING_BOX_SIZE * (0.9f * trans.scale.y) + trans.position.y };
			box.max = { BOUNDING_BOX_SIZE * (0.6f * trans.scale.x) + trans.position.x,
						BOUNDING_BOX_SIZE * (0.9f * trans.scale.y) + trans.position.y };
		}
		else
		{
			box.min = { -BOUNDING_BOX_SIZE * trans.scale.x + trans.position.x,
						-BOUNDING_BOX_SIZE * trans.scale.y + trans.position.y };
			box.max = { BOUNDING_BOX_SIZE * trans.scale.x + trans.position.x,
						BOUNDING_BOX_SIZE * trans.scale.y + trans.position.y };
		}
	}

	/*!***********************************************************************
	\brief
	 Check for collision detection between two boxes.
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
	 Check for collision detection between a box and the screen boundaries.
	\param[in/out] box
	 The box to collide.
	\return
	 Collision flags if the box and screen boundary intersect, no flag otherwise.
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
	 Check for collision detection between two circles.
	\param[in] circle1
	 The first circle.
	\param[in] circle2
	 The second circle.
	\return
	 True if the circles intersect, false otherwise.
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
	 Check for collision detection between a circle and a box.
	\param[in] circle
	 The circle to collide.
	\param[in] box
	 The box to collide.
	\return
	 True if the circle and box intersect, false otherwise.
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
	\param[in] convex1
	 The first convex.
	\param[in] convex2
	 The second convex.
	\return
	 True if the two convex intersect, false otherwise.
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
	\param[in] convex
	 The convex collider to project.
	\param[in] axis
	 The axis which the convex vertices are projected.
	\param[out] min
	 The minimum projection value.
	\param[out] max
	 The maximum projection value.
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
	\param[in/out] entity1
	 The first collided object.
	\param[in/out] entity2
	 The second collided object.
	\param[in] firstTimeOfCollision
	 The calculated first time of collision.
	*************************************************************************/
	void Collision::BoxBox_Response(const EntityID& entity1, const EntityID& entity2, float firstTimeOfCollision)
	{
		// Get the tag of the first entity
		std::string tag1 = GameObjectManager::GetInstance().GetGO(entity1)->GetTag();

		// Get references of the first entity components
		auto& trans1 = ECS::GetInstance().GetComponent<Transform>(entity1);
		auto& box1 = ECS::GetInstance().GetComponent<BoxCollider2D>(entity1);
		auto& rb1 = ECS::GetInstance().GetComponent<Rigidbody2D>(entity1);

		// Get the tag of the second entity
		std::string tag2 = GameObjectManager::GetInstance().GetGO(entity2)->GetTag();

		// Get references of the second entity components
		auto& trans2 = ECS::GetInstance().GetComponent<Transform>(entity2);
		auto& box2 = ECS::GetInstance().GetComponent<BoxCollider2D>(entity2);
		auto& rb2 = ECS::GetInstance().GetComponent<Rigidbody2D>(entity2);

		if (tag1 == "Player" && box2.is_trigger)
		{
			// Mochi and Door / Other Triggers
			Trigger_Response(tag2);
		}
		else if (tag1 == "Knife" && tag2 == "Enemy")
		{
			// Mochi's Knife and Enemy
			// Enemy takes damage and knockback

			// Get references of the player and enemy
			auto& player_data = ECS::GetInstance().GetComponent<Player>(player);
			auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);
			auto& player_anim = ECS::GetInstance().GetComponent<Animation>(player);
			auto& enemy_data = ECS::GetInstance().GetComponent<Enemy>(entity2);

			if (player_anim.currentClip != "Attack" && player_anim.currentClip != "bAttack" && player_anim.currentClip != "rAttack")
				return;

			switch (player_data.comboState)
			{
			case 0: // First combo state
				if (player_anim.current_frame == 8)
				{
					if (!enemy_data.hasDealtDamage) //HAS TAKEN DMG
					{
						ECS::GetInstance().GetComponent<Animation>(entity2).SetAnimationUninterrupted("Hurt");
						enemy_data.atktimer = 2.0f;

						// Deal 2x dmg if the player and the enemy has the same soul type
						if (player_soul.current_soul == enemy_data.type)
							enemy_data.TakeDamage(static_cast<float>(player_data.comboDamage * 2.f));
						else
							enemy_data.TakeDamage(static_cast<float>(player_data.comboDamage));

						enemy_data.hasDealtDamage = true; // Prevent multiple applications
					}
				}
				else
				{
					// Reset damage flag for the kick combo if not at the damage frame
					enemy_data.hasDealtDamage = false;
				}
				break;

			case 1: // Second combo state
				if (player_anim.current_frame == 23)
				{
					if (!enemy_data.hasDealtDamage)
					{
						ECS::GetInstance().GetComponent<Animation>(entity2).SetAnimationUninterrupted("Hurt");
						enemy_data.atktimer = 2.0f;

						// Deal 2x dmg if the player and the enemy has the same soul type
						if (player_soul.current_soul == enemy_data.type)
							enemy_data.TakeDamage(static_cast<float>(player_data.comboDamage * 2.f));
						else
							enemy_data.TakeDamage(static_cast<float>(player_data.comboDamage));

						enemy_data.hasDealtDamage = true; // Prevent multiple applications
					}
				}
				else
				{
					// Reset damage flag for the kick combo if not at the damage frame
					enemy_data.hasDealtDamage = false;
				}
				break;

			case 2: // Knockback kick combo
				if (player_anim.current_frame == 29)
				{
					// Apply knockback and play sound effects
					auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<
						AudioManager>();
					ECS::GetInstance().GetSystem<Physics>()->ApplyKnockback(trans1, 150000, trans2, rb2);

					if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(
						audioM.GetSFXindex("Pattack3")))
					{
						audioM.PlaySFX(audioM.GetSFXindex("Pattack3"));
					}
					enemy_data.isKick = true;

					// Deal damage during the knockback kick
					if (!enemy_data.hasDealtDamage)
					{
						enemy_data.atktimer = 2.0f;
						
						// Deal 2x dmg if the player and the enemy has the same soul type
						if (player_soul.current_soul == enemy_data.type)
							enemy_data.TakeDamage(static_cast<float>(player_data.comboDamage * 2.f));
						else
							enemy_data.TakeDamage(static_cast<float>(player_data.comboDamage));

						enemy_data.hasDealtDamage = true;
					}
				}
				else
				{
					// Reset damage flag for the kick combo if not at the damage frame
					enemy_data.hasDealtDamage = false;
				}
				break;

			default:
				break;
			}
		}
		else if ((tag1 == "FishAbility" || tag1 == "WormAbility") && tag2 == "Enemy")
		{
			// Mochi's Ability and Enemy
			// Enemy takes damage and knockback

			// Get references of the player and enemy
			auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);
			auto& enemy_data = ECS::GetInstance().GetComponent<Enemy>(entity2);

			if (!enemy_data.hasDealtDamage)
			{
				ECS::GetInstance().GetComponent<Animation>(entity2).SetAnimationUninterrupted("Hurt");
				enemy_data.atktimer = 2.0f;

				// Deal 2x dmg if the player and the enemy has the same soul type
				if (player_soul.current_soul == enemy_data.type)
					enemy_data.TakeDamage(player_soul.skill_damages[player_soul.current_soul] * 2.f);
				else
					enemy_data.TakeDamage(player_soul.skill_damages[player_soul.current_soul]);
				
				enemy_data.hasDealtDamage = true; // Prevent multiple applications
			}
			else
			{
				// Reset damage flag for the kick combo if not at the damage frame
				enemy_data.hasDealtDamage = false;
			}
		}
		else if (tag1 == "Knife" && tag2 == "EnemyProjectile" || tag1 == "FishAbility" && tag2 == "EnemyProjectile"
			|| tag1 == "WormAbility" && tag2 == "EnemyProjectile" || tag1 == "Environment" && tag2 == "EnemyProjectile")
		{
			// Mochi's Knife / Mochi's Ability / Environment Objects and Enemy's Projectile
			// Destroy enemy's projectile
		}

		// Skip trigger objects
		if (box1.is_trigger || box2.is_trigger)
			return;

		if (tag1 == "Player" && tag2 == "Enemy" || tag1 == "Player" && tag2 == "EnemyProjectile")
		{
			// Mochi and Enemy / Enemy's Projectile
			// Mochi takes damage and knockback

			// Get references of the player and enemy
			auto& player_data = ECS::GetInstance().GetComponent<Player>(player);
			if (tag2 == "Enemy")
			{
				auto& enemy_data = ECS::GetInstance().GetComponent<Enemy>(entity2);
				// Deal damage to the player
				//enemy_data.AttackPlayer(player_data.currentHealth);
			}
			else if (tag2 == "EnemyProjectile" && ECS::GetInstance().HasComponent<EnemyBullet>(entity2))
			{
				auto& bullet_data = ECS::GetInstance().GetComponent<EnemyBullet>(entity2);
				if (!bullet_data.hit)
				{
					player_data.currentHealth -= 10.f;
					bullet_data.hit = true;
				}

			}

			// STATIC AND DYNAMIC / DYNAMIC AND DYNAMIC
			//Static_Response(trans1, box1, rb1, trans2, box2, rb2);
			//StaticDynamic_Response(trans1, box1, rb1, trans2, box2, rb2, firstTimeOfCollision);

			// Play a sound effect on collision
			//if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
			//{
			//	auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
			//	
			//	if (audioM.GetSFXindex("PlayerHurt") != -1)
			//	{
			//		if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("PlayerHurt")))
			//			audioM.PlaySFX(audioM.GetSFXindex("PlayerHurt"));
			//	}
			//}
		}
		else if (tag1 == "Player" && tag2 == "Environment" || tag1 == "Player" && tag2 == "Boundary")
		{
			// Mochi and Environment Objects / Boundaries
			// Acts as a wall

			// STATIC AND DYNAMIC
			Static_Response(trans1, box1, rb1, trans2, box2, rb2);
			StaticDynamic_Response(trans1, box1, rb1, trans2, box2, rb2, firstTimeOfCollision);

			// Play a sound effect on collision
			if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
			{
				auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
				if (audioM.GetSFXindex("HIT") != -1)
				{
					if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("HIT")))
						audioM.PlaySFX(audioM.GetSFXindex("HIT"));
				}
			}
		}
		else if (tag1 == "Enemy" && tag2 == "Environment" || tag1 == "Enemy" && tag2 == "Boundary")
		{
			// Enemy and Environment Objects / Boundaries
			// Acts as a wall

			ECS::GetInstance().GetSystem<EnemyManager>()->EnemyCollisionResponse(entity1, entity2);

			// STATIC AND DYNAMIC
			Static_Response(trans1, box1, rb1, trans2, box2, rb2);
			StaticDynamic_Response(trans1, box1, rb1, trans2, box2, rb2, firstTimeOfCollision);
		}
		else if (tag1 == "Enemy" && tag2 == "Enemy")
		{
			// Enemy and Enemy
			// Block each other
			return;

			ECS::GetInstance().GetSystem<EnemyManager>()->EnemyCollisionResponse(entity1, entity2);
			ECS::GetInstance().GetSystem<Physics>()->ApplyKnockback(trans1, 15000, trans2, rb2);
			ECS::GetInstance().GetSystem<Physics>()->ApplyKnockback(trans2, 15000, trans1, rb1);

			// STATIC AND DYNAMIC / DYNAMIC AND DYNAMIC
			//Static_Response(trans1, box1, rb1, trans2, box2, rb2);
			//StaticDynamic_Response(trans1, box1, rb1, trans2, box2, rb2, firstTimeOfCollision);

			auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
			if (audioM.GetSFXindex("HIT") != -1)
			{
				if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("HIT")))
					audioM.PlaySFX(audioM.GetSFXindex("HIT"));
			}
		}
	}

	/*!***********************************************************************
	\brief
	 Collision response between a static object and a dynamic object, and between two dynamic objects.
	\param[out] trans1
	 The Transform component attached to the first collided object.
	\param[in] box1
	 The BoxCollider2D component attached to the first collided object.
	\param[out] rb1
	 The Rigidbody2D component attached to the first collided object.
	\param[out] trans2
	 The Transform component attached to the second collided object.
	\param[in] box2
	 The BoxCollider2D component attached to the second collided object.
	\param[out] rb2
	 The Rigidbody2D component attached to the second collided object.
	\param[in] firstTimeOfCollision
	 The calculated first time of collision.
	*************************************************************************/
	void Collision::StaticDynamic_Response(Transform& trans1, BoxCollider2D& box1, Rigidbody2D& rb1, Transform& trans2, BoxCollider2D& box2, Rigidbody2D& rb2, float firstTimeOfCollision)
	{
		// Move objects to the point of collision
		if (!rb1.is_kinematic)
		{
			Vec2 new_value = rb1.velocity * firstTimeOfCollision * static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
			trans1.position.x += new_value.x;
			trans1.position.y += new_value.y;
		}
		if (!rb2.is_kinematic)
		{
			Vec2 new_value = rb2.velocity * firstTimeOfCollision * static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
			trans2.position.x += new_value.x;
			trans2.position.y += new_value.y;
		}

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
	\param[out] trans1
	 The Transform component attached to the first collided object.
	\param[in] box1
	 The BoxCollider2D component attached to the first collided object.
	\param[in] rb1
	 The Rigidbody2D component attached to the first collided object.
	\param[out] trans2
	 The Transform component attached to the second collided object.
	\param[in] box2
	 The BoxCollider2D component attached to the second collided object.
	\param[in] rb2
	 The Rigidbody2D component attached to the second collided object.
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

		//// Box 1 top and box 2 bottom collision response
		//if (box1.collision_flag & COLLISION_TOP && box2.collision_flag & COLLISION_BOTTOM)
		//{
		//	// To simulate floor/ceiling collision
		//	if (!rb1.is_kinematic)
		//		trans1.position.y = box2.max.y + trans1.scale.y * 0.5f + MIN_OFFSET; // Move box1 upwards
		//	if (!rb2.is_kinematic)
		//		trans2.position.y = box1.min.y - trans2.scale.y * 0.5f - MIN_OFFSET; // Move box2 downwards
		//}

		//// Box 1 bottom and box 2 top collision response
		//if (box1.collision_flag & COLLISION_BOTTOM && box2.collision_flag & COLLISION_TOP)
		//{
		//	// To simulate floor/ceiling collision
		//	if (!rb1.is_kinematic)
		//		trans1.position.y = box2.min.y - trans1.scale.y * 0.5f - MIN_OFFSET; // Move box1 downwards
		//	if (!rb2.is_kinematic)
		//		trans2.position.y = box1.max.y + trans2.scale.y * 0.5f + MIN_OFFSET; // Move box2 upwards
		//}


		// Box 1 top and box 2 bottom collision response
		if (box1.collision_flag & COLLISION_TOP && box2.collision_flag & COLLISION_BOTTOM)
		{
			float overlap = box2.max.y - box1.min.y; // Calculate overlap depth
			if (!rb1.is_kinematic)
				trans1.position.y += overlap + MIN_OFFSET; // Move box1 out of the collision
			if (!rb2.is_kinematic)
				trans2.position.y -= overlap + MIN_OFFSET; // Move box2 out of the collision
		}

		// Box 1 bottom and box 2 top collision response
		if (box1.collision_flag & COLLISION_BOTTOM && box2.collision_flag & COLLISION_TOP)
		{
			float overlap = box1.max.y - box2.min.y; // Calculate overlap depth
			if (!rb1.is_kinematic)
				trans1.position.y -= overlap + MIN_OFFSET; // Move box1 out of the collision
			if (!rb2.is_kinematic)
				trans2.position.y += overlap + MIN_OFFSET; // Move box2 out of the collision
		}
	}

	/*!***********************************************************************
	\brief
	 Collision response between two dynamic objects.
	\param[out] trans1
	 The Transform component attached to the first collided object.
	\param[out] rb1
	 The Rigidbody2D component attached to the first collided object.
	\param[out] trans2
	 The Transform component attached to the second collided object.
	\param[out] rb2
	 The Rigidbody2D component attached to the second collided object.
	*************************************************************************/
	void Collision::Dynamic_Response(Transform& trans1, Rigidbody2D& rb1, Transform& trans2, Rigidbody2D& rb2)
	{
		// DYNAMIC AND DYNAMIC
		// Calculate the difference in positions
		Vector3D difference = trans2.position - trans1.position;
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
	\param[in] trigger_tag
	 The tag of the trigger object.
	*************************************************************************/
	void Collision::Trigger_Response(const std::string& trigger_tag)
	{
		auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
		if (audioM.GetSFXindex("LevelChange") != -1)
		{
			if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("LevelChange")))
				audioM.PlaySFX(audioM.GetSFXindex("LevelChange"));
		}
		// PLAYER AND DOORS
		if (trigger_tag == "LeftDoor")
		{
			// Move to the left room
			ECS::GetInstance().GetSystem<DungeonManager>()->SwitchToRoom(-1);
		}
		else if (trigger_tag == "RightDoor")
		{
			// Move to the right room
			ECS::GetInstance().GetSystem<DungeonManager>()->SwitchToRoom(1);
		}
		//else if (trigger_tag == "TopDoor")
		//{
		//	// Move to the top room
		//}
		//else if (trigger_tag == "BtmDoor")
		//{
		//	// Move to the bottom room
		//}

		// PLAYER AND OTHER TRIGGERS (coins, checkpoints, etc..)
	}

	/*!***********************************************************************
	\brief
	 Collision response between an object and the screen boundaries.
	\param[out] trans
	 The Transform component attached to the collided object.
	\param[in] box
	 The BoxCollider2D component attached to the collided object.
	*************************************************************************/
	void Collision::BoxScreen_Response(Transform& trans, const BoxCollider2D& box)
	{
		// Colliding with the left screen boundary
		if (box.collision_flag & COLLISION_LEFT)
		{
			trans.position.x = trans.scale.x * 0.5f;
		}

		// Colliding with the right screen boundary
		if (box.collision_flag & COLLISION_RIGHT)
		{
			trans.position.x = screen_width - trans.scale.x * 0.5f;
		}

		// Colliding with the top screen boundary
		if (box.collision_flag & COLLISION_TOP)
		{
			trans.position.y = trans.scale.y * 0.5f;
		}

		// Colliding with the bottom screen boundary
		if (box.collision_flag & COLLISION_BOTTOM)
		{
			trans.position.y = screen_height - trans.scale.y * 0.5f;
		}
	}
}