/* Start Header ************************************************************************/
/*!
\file       Collision.cpp
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Mar 27, 2025
\brief      This file contains the definition of the Collision system.

Copyright (C) 2025 DigiPen Institute of Technology.
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
#include "../Game/SoulManager.h"				// for HarvestSoul
#include "../SceneManager.h"					// for cheat mode
#include "../Game/BossManager.h"				// for Boss

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
		for (int step = 0; step < g_FrameRateController.GetCurrentNumberOfSteps(); ++step)
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
				// Skip if the first entity is not active
				if (!GameObjectManager::GetInstance().GetGO(entity1)->GetActive())
					continue;

				// Get the tag of the first entity
				std::string tag1 = GameObjectManager::GetInstance().GetGO(entity1)->GetTag();

				// Get references of the first entity components
				auto& trans1 = ECS::GetInstance().GetComponent<Transform>(entity1);
				auto& box1 = ECS::GetInstance().GetComponent<BoxCollider2D>(entity1);

				if (!ECS::GetInstance().HasComponent<Rigidbody2D>(entity1))
					return;
				auto& rb1 = ECS::GetInstance().GetComponent<Rigidbody2D>(entity1);

				// Update the bounding box size
				UpdateBoundingBox(box1, trans1, tag1);

				// Get the vector of potential collisions within the quad
				std::vector<EntityID> potential_collisions;
				quadtree->Retrieve(potential_collisions, box1);

				for (auto const& entity2 : potential_collisions)
				{
					// Skip self collision
					if (entity1 == entity2)
						continue;

					// Skip if the second entity is not active
					if (!GameObjectManager::GetInstance().GetGO(entity2)->GetActive())
						continue;

					// Get the tag of the second entity
					std::string tag2 = GameObjectManager::GetInstance().GetGO(entity2)->GetTag();

					// Get references of the second entity components
					auto& trans2 = ECS::GetInstance().GetComponent<Transform>(entity2);
					auto& box2 = ECS::GetInstance().GetComponent<BoxCollider2D>(entity2);
					auto& rb2 = ECS::GetInstance().GetComponent<Rigidbody2D>(entity2);

					// Update the bounding box size
					UpdateBoundingBox(box2, trans2, tag2);

					// Perform narrow-phase collision checks, check collision between two box objects
					float tLast{};
					if (BoxBox_Intersection(box1, rb1.velocity, box2, rb2.velocity, tLast))
						BoxBox_Response(entity1, entity2, tLast);
				}
			}
		}
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
		if (tag == "Player" || tag == "Enemy")
		{
			// Set bounding box to half in the y-axis and shifted down
			box.min = { trans.position.x - trans.scale.x * BOUNDING_BOX_SIZE,
						trans.position.y - trans.scale.y * BOUNDING_BOX_SIZE };
			box.max = { trans.position.x + trans.scale.x * BOUNDING_BOX_SIZE,
						trans.position.y - trans.scale.y * 0.25f };
		}
		else if (tag == "Environment")
		{
			// Set bounding box to half in the y-axis and shifted down
			box.min = { trans.position.x - trans.scale.x * BOUNDING_BOX_SIZE,
						trans.position.y - trans.scale.y * 0.6f };
			box.max = { trans.position.x + trans.scale.x * BOUNDING_BOX_SIZE,
						trans.position.y - trans.scale.y * 0.25f };
		}
		else if (tag == "Dummy")
		{
			// Set bounding box to half in the x-axis and y-axis and shifted down
			box.min = { trans.position.x - trans.scale.x * 0.25f,
						trans.position.y - trans.scale.y * BOUNDING_BOX_SIZE };
			box.max = { trans.position.x + trans.scale.x * 0.25f,
						trans.position.y - trans.scale.y * 0.25f };
		}
		else if (tag == "Knife")
		{
			// Set bounding box to be longer in the x-axis and shifted down
			box.min = { trans.position.x - trans.scale.x * 0.75f,
						trans.position.y - trans.scale.y * 0.625f };
			box.max = { trans.position.x + trans.scale.x * 0.9f,
						trans.position.y - trans.scale.y * 0.4f };
		}
		else if (tag == "EnemyProjectile")
		{
			// Set bounding box to be longer in the y-axis
			box.min = { trans.position.x - trans.scale.x * BOUNDING_BOX_SIZE,
						trans.position.y - trans.scale.y * 4.f };
			box.max = { trans.position.x + trans.scale.x * BOUNDING_BOX_SIZE,
						trans.position.y + trans.scale.y * BOUNDING_BOX_SIZE };
		}
		else if (tag == "Hair")
		{
			// Set bounding box to be longer in the x-axis
			box.min = { trans.position.x - trans.scale.x * 1.5f,
						trans.position.y - trans.scale.y * BOUNDING_BOX_SIZE };
			box.max = { trans.position.x + trans.scale.x * 1.5f,
						trans.position.y + trans.scale.y * BOUNDING_BOX_SIZE };
		}
		else
		{
			// Set bounding box to full AABB size
			box.min = { trans.position.x - trans.scale.x * BOUNDING_BOX_SIZE,
						trans.position.y - trans.scale.y * BOUNDING_BOX_SIZE };
			box.max = { trans.position.x + trans.scale.x * BOUNDING_BOX_SIZE,
						trans.position.y + trans.scale.y * BOUNDING_BOX_SIZE };
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

		if (tag1 == "Player" && tag2 == "LeftDoor" || tag1 == "Player" && tag2 == "RightDoor") // Mochi and Doors
		{
			if (tag2 == "LeftDoor")
				ECS::GetInstance().GetSystem<DungeonManager>()->SwitchToRoom(-1); // Move to the left room
			else if (tag2 == "RightDoor")
				ECS::GetInstance().GetSystem<DungeonManager>()->SwitchToRoom(1); // Move to the right room

			//else if (tag2 == "TopDoor")
			//	// Move to the top room
			//else if (tag2 == "BtmDoor")
			//	// Move to the bottom room
		}
		else if (tag1 == "Knife" && tag2 == "Enemy") // Mochi's Knife and Enemy (Enemy takes damage and knockback)
		{
			// Get references of the player and enemy
			auto& player_trans = ECS::GetInstance().GetComponent<Transform>(player);
			auto& player_sr = ECS::GetInstance().GetComponent<SpriteRender>(player);
			auto& player_data = ECS::GetInstance().GetComponent<Player>(player);
			auto& player_soul = ECS::GetInstance().GetComponent<PlayerSoul>(player);
			auto& player_anim = ECS::GetInstance().GetComponent<Animation>(player);
			auto& enemy_data = ECS::GetInstance().GetComponent<Enemy>(entity2);
			auto& enemy_sr = ECS::GetInstance().GetComponent<SpriteRender>(entity2);
			auto& vfxhit_trans = GameObjectManager::GetInstance().GetGOByName("Hit_Effect")->GetComponent<Transform>();
			auto& vfxhit_anim = GameObjectManager::GetInstance().GetGOByName("Hit_Effect")->GetComponent<Animation>();

			if (enemy_data.state == Enemy::DEAD)
			{
				return;
			}

			// Skip if Mochi is not attacking
			if (player_anim.currentClip != "Attack" && player_anim.currentClip != "bAttack" && player_anim.currentClip != "rAttack")
				return;

			switch (player_data.comboState)
			{
			case 0: // First combo state
				if (player_anim.current_frame == 10)
				{
					if (!enemy_data.hasDealtDamage) //HAS TAKEN DMG
					{
						ECS::GetInstance().GetComponent<Animation>(entity2).SetAnimationUninterrupted("Hurt");
						enemy_data.atktimer = 1.5f;

						// Deal 2x dmg if the player and the enemy has the same soul type
						if (player_soul.current_soul == enemy_data.type)
							enemy_data.TakeDamage(static_cast<float>(player_data.comboDamage * 2.f));
						else
							enemy_data.TakeDamage(static_cast<float>(player_data.comboDamage));

						// Harvest some soul whenever mochi hits an enemy
						ECS::GetInstance().GetSystem<SoulManager>()->HarvestSoul(static_cast<SoulType>(enemy_data.type), 5.f);

						enemy_data.hasDealtDamage = true; // Prevent multiple applications
						enemy_sr.color = Vec3(1.f, 0.f, 0.f);

						if (player_sr.flipX)
						{
							vfxhit_trans.position = Vector3D(player_trans.position.x + 150.0f, player_trans.position.y, 0);
							vfxhit_anim.RestartAnimation();
						}
						else
						{
							vfxhit_trans.position = Vector3D(player_trans.position.x - 150.0f, player_trans.position.y, 0);
							vfxhit_anim.RestartAnimation();
						}
						player_data.HitStopAnimation();
					}
				}
				else
				{
					// Reset damage flag for the kick combo if not at the damage frame
					enemy_data.hasDealtDamage = false;
				}
				break;

			case 1: // Second combo state
				if (player_anim.current_frame == 15)
				{
					if (!enemy_data.hasDealtDamage)
					{
						ECS::GetInstance().GetComponent<Animation>(entity2).SetAnimationUninterrupted("Hurt");

						// Deal 2x dmg if the player and the enemy has the same soul type
						if (player_soul.current_soul == enemy_data.type)
							enemy_data.TakeDamage(static_cast<float>(player_data.comboDamage * 2.f));
						else
							enemy_data.TakeDamage(static_cast<float>(player_data.comboDamage));

						// Harvest some soul whenever mochi hits an enemy
						ECS::GetInstance().GetSystem<SoulManager>()->HarvestSoul(static_cast<SoulType>(enemy_data.type), 5.f);
						enemy_data.hasDealtDamage = true; // Prevent multiple applications
						enemy_sr.color = Vec3(1.f, 0.f, 0.f);

						if (player_sr.flipX)
						{
							vfxhit_trans.position = Vector3D(player_trans.position.x + 150.0f, player_trans.position.y, 0);
							vfxhit_anim.RestartAnimation();
						}
						else
						{
							vfxhit_trans.position = Vector3D(player_trans.position.x - 150.0f, player_trans.position.y, 0);
							vfxhit_anim.RestartAnimation();
						}
						player_data.HitStopAnimation();
					}
				}
				else
				{
					// Reset damage flag for the kick combo if not at the damage frame
					enemy_data.hasDealtDamage = false;
				}
				break;

			case 2: // Knockback kick combo
				if (player_anim.current_frame == 31)
				{
					// Apply knockback and play sound effects
					ECS::GetInstance().GetSystem<Physics>()->ApplyKnockback(trans1, 150000, trans2, rb2);
					
					if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
					{
						auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();

						if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("Pattack3")))
						{
							audioM.PlaySFX(audioM.GetSFXindex("Pattack3"));
						}
					}
					enemy_data.isKick = true;

					// Deal damage during the knockback kick
					if (!enemy_data.hasDealtDamage)
					{
						ECS::GetInstance().GetComponent<Animation>(entity2).SetAnimationUninterrupted("Hurt");
						//enemy_data.atktimer = 1.5f;

						// Deal 2x dmg if the player and the enemy has the same soul type
						if (player_soul.current_soul == enemy_data.type)
							enemy_data.TakeDamage(static_cast<float>(player_data.comboDamage * 2.f));
						else
							enemy_data.TakeDamage(static_cast<float>(player_data.comboDamage));

						// Harvest some soul whenever mochi hits an enemy
						ECS::GetInstance().GetSystem<SoulManager>()->HarvestSoul(static_cast<SoulType>(enemy_data.type), 5.f);

						enemy_data.hasDealtDamage = true;
						enemy_sr.color = Vec3(1.f, 0.f, 0.f);

						if (player_sr.flipX)
						{
							vfxhit_trans.position = Vector3D(player_trans.position.x + 150.0f, player_trans.position.y, 0);
							vfxhit_anim.RestartAnimation();
						}
						else
						{
							vfxhit_trans.position = Vector3D(player_trans.position.x - 150.0f, player_trans.position.y, 0);
							vfxhit_anim.RestartAnimation();
						}
						player_data.HitStopAnimation();
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
		else if (tag1 == "Knife" && tag2 == "Environment") // Mochi's Knife and Environment (Mochi heals from breaking boxes)
		{
			// Skip if box is already a trigger/broken
			if (box2.is_trigger)
				return;

			// Get references of the player and box
			auto& player_trans = ECS::GetInstance().GetComponent<Transform>(player);
			auto& player_sr = ECS::GetInstance().GetComponent<SpriteRender>(player);
			auto& player_data = ECS::GetInstance().GetComponent<Player>(player);
			auto& player_anim = ECS::GetInstance().GetComponent<Animation>(player);
			auto& box_anim = ECS::GetInstance().GetComponent<Animation>(entity2);
			auto& vfxhit_trans = GameObjectManager::GetInstance().GetGOByName("Hit_Effect")->GetComponent<Transform>();
			auto& vfxhit_anim = GameObjectManager::GetInstance().GetGOByName("Hit_Effect")->GetComponent<Animation>();

			// Skip if Mochi is not attacking
			if (player_anim.currentClip != "Attack" && player_anim.currentClip != "bAttack" && player_anim.currentClip != "rAttack")
				return;

			switch (player_data.comboState)
			{
			case 0: // First combo state
				if (player_anim.current_frame == 10)
				{
					if (!check_collision_once)
					{
						if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
						{
							auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();

							// Randomize box hit sounds
							std::vector<int> BoxHitSounds = {
								audioM.GetSFXindex("HitBoxSmash1"),
								audioM.GetSFXindex("HitBoxSmash2"),
								audioM.GetSFXindex("HitBoxSmash3"),
								audioM.GetSFXindex("HitBoxSmash4"),
								audioM.GetSFXindex("HitBoxSmash5"),
								audioM.GetSFXindex("HitBoxSmash6"),
								audioM.GetSFXindex("HitBoxSmash7"),
								audioM.GetSFXindex("HitBoxSmash8")
							};

							// Remove invalid (-1) sounds
							BoxHitSounds.erase(
								std::remove(BoxHitSounds.begin(), BoxHitSounds.end(), -1),
								BoxHitSounds.end()
							);

							if (!BoxHitSounds.empty())
							{
								// Randomly select a hit sound
								int randomIndex = rand() % BoxHitSounds.size();
								int selectedSound = BoxHitSounds[randomIndex];

								// Play the selected sound
								audioM.PlaySFX(selectedSound);
							}
						}

						if (box_anim.currentClip == "Box")
						{
							box_anim.SetAnimation("Box_Break");
							box_anim.is_playing = false;
							box_anim.current_frame = 0;
						}
						else if (box_anim.currentClip == "Box_Break")
						{
							int prev_frame = box_anim.current_frame;
							box_anim.current_frame++;
							if (box_anim.current_frame > 3)
							{
								box_anim.current_frame = 3;
							}

							if (prev_frame == 2 && box_anim.current_frame == 3)
							{
								player_data.currentHealth += 10;
								player_data.postInjuriesMaxHealth += 10;
								if (player_data.currentHealth > player_data.maxHealth)
								{
									player_data.currentHealth = player_data.maxHealth;
								}
								if (player_data.postInjuriesMaxHealth > player_data.maxHealth)
								{
									player_data.postInjuriesMaxHealth = player_data.maxHealth;
								}

								box2.is_trigger = true;
							}
						}

						if (player_sr.flipX)
						{
							vfxhit_trans.position = Vector3D(player_trans.position.x + 150.0f, player_trans.position.y, 0);
							vfxhit_anim.RestartAnimation();
						}
						else
						{
							vfxhit_trans.position = Vector3D(player_trans.position.x - 150.0f, player_trans.position.y, 0);
							vfxhit_anim.RestartAnimation();
						}
						player_data.HitStopAnimation();

						check_collision_once = true;
					}
					else
						check_collision_once = false;
				}
				break;

			case 1: // Second combo state
				if (player_anim.current_frame == 15)
				{
					if (!check_collision_once)
					{
						if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
						{
							auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();

							// Randomize box hit sounds
							std::vector<int> BoxHitSounds = {
								audioM.GetSFXindex("HitBoxSmash1"),
								audioM.GetSFXindex("HitBoxSmash2"),
								audioM.GetSFXindex("HitBoxSmash3"),
								audioM.GetSFXindex("HitBoxSmash4"),
								audioM.GetSFXindex("HitBoxSmash5"),
								audioM.GetSFXindex("HitBoxSmash6"),
								audioM.GetSFXindex("HitBoxSmash7"),
								audioM.GetSFXindex("HitBoxSmash8")
							};

							// Remove invalid (-1) sounds
							BoxHitSounds.erase(
								std::remove(BoxHitSounds.begin(), BoxHitSounds.end(), -1),
								BoxHitSounds.end()
							);

							if (!BoxHitSounds.empty())
							{
								// Randomly select a hit sound
								int randomIndex = rand() % BoxHitSounds.size();
								int selectedSound = BoxHitSounds[randomIndex];

								// Play the selected sound
								audioM.PlaySFX(selectedSound);
							}
						}
						if (box_anim.currentClip == "Box")
						{
							box_anim.SetAnimation("Box_Break");
							box_anim.is_playing = false;
							box_anim.current_frame = 0;
						}
						else if (box_anim.currentClip == "Box_Break")
						{
							int prev_frame = box_anim.current_frame;
							box_anim.current_frame++;
							if (box_anim.current_frame > 3)
							{
								box_anim.current_frame = 3;
							}
							if (prev_frame == 2 && box_anim.current_frame == 3)
							{
								player_data.currentHealth += 10;
								player_data.postInjuriesMaxHealth += 10;
								if (player_data.currentHealth > player_data.maxHealth)
								{
									player_data.currentHealth = player_data.maxHealth;
								}
								if (player_data.postInjuriesMaxHealth > player_data.maxHealth)
								{
									player_data.postInjuriesMaxHealth = player_data.maxHealth;
								}

								box2.is_trigger = true;
							}
						}

						if (player_sr.flipX)
						{
							vfxhit_trans.position = Vector3D(player_trans.position.x + 150.0f, player_trans.position.y, 0);
							vfxhit_anim.RestartAnimation();
						}
						else
						{
							vfxhit_trans.position = Vector3D(player_trans.position.x - 150.0f, player_trans.position.y, 0);
							vfxhit_anim.RestartAnimation();
						}
						player_data.HitStopAnimation();

						check_collision_once = true;
					}
					else
						check_collision_once = false;
				}
				break;

			case 2: // Knockback kick combo
				if (player_anim.current_frame == 31)
				{
					if (!check_collision_once)
					{
						if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
						{
							auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();

							// Randomize box smash sounds (using case 2's sounds)
							std::vector<int> BoxSmashSounds = {
								audioM.GetSFXindex("BoxSmash1"),
								audioM.GetSFXindex("BoxSmash2"),
								audioM.GetSFXindex("BoxSmash3"),
								audioM.GetSFXindex("BoxSmash4")
							};

							// Remove invalid (-1) sounds
							BoxSmashSounds.erase(
								std::remove(BoxSmashSounds.begin(), BoxSmashSounds.end(), -1),
								BoxSmashSounds.end()
							);

							if (!BoxSmashSounds.empty())
							{
								// Randomly select a hit sound
								int randomIndex = rand() % BoxSmashSounds.size();
								int selectedSound = BoxSmashSounds[randomIndex];

								// Play the selected sound
								audioM.PlaySFX(selectedSound);
							}
						}
						if (box_anim.currentClip == "Box")
						{
							box_anim.SetAnimation("Box_Break");
							box_anim.is_playing = false;
							box_anim.current_frame = 0;
						}
						else if (box_anim.currentClip == "Box_Break")
						{
							int prev_frame = box_anim.current_frame;
							box_anim.current_frame++;
							if (box_anim.current_frame > 3)
							{
								box_anim.current_frame = 3;
							}
							if (prev_frame == 2 && box_anim.current_frame == 3)
							{
								player_data.currentHealth += 10;
								player_data.postInjuriesMaxHealth += 10;
								if (player_data.currentHealth > player_data.maxHealth)
								{
									player_data.currentHealth = player_data.maxHealth;
								}
								if (player_data.postInjuriesMaxHealth > player_data.maxHealth)
								{
									player_data.postInjuriesMaxHealth = player_data.maxHealth;
								}

								box2.is_trigger = true;
							}
						}

						if (player_sr.flipX)
						{
							vfxhit_trans.position = Vector3D(player_trans.position.x + 150.0f, player_trans.position.y, 0);
							vfxhit_anim.RestartAnimation();
						}
						else
						{
							vfxhit_trans.position = Vector3D(player_trans.position.x - 150.0f, player_trans.position.y, 0);
							vfxhit_anim.RestartAnimation();
						}
						player_data.HitStopAnimation();

						check_collision_once = true;
					}
					else
						check_collision_once = false;
				}
				break;

			default:
				break;
			}
		}
		else if (tag1 == "Knife" && tag2 == "Hair") // Mochi's Knife and Boss Hair (Boss hair takes damage)
		{
			// Get references of the player and enemy boss
			auto& player_trans = ECS::GetInstance().GetComponent<Transform>(player);
			auto& player_sr = ECS::GetInstance().GetComponent<SpriteRender>(player);
			auto& player_data = ECS::GetInstance().GetComponent<Player>(player);
			auto& player_anim = ECS::GetInstance().GetComponent<Animation>(player);
			auto& vfxhit_trans = GameObjectManager::GetInstance().GetGOByName("Hit_Effect")->GetComponent<Transform>();
			auto& vfxhit_anim = GameObjectManager::GetInstance().GetGOByName("Hit_Effect")->GetComponent<Animation>();
			auto& boss_sr = ECS::GetInstance().GetComponent<SpriteRender>(entity2);

			// Skip if Mochi is not attacking
			if (player_anim.currentClip != "Attack" && player_anim.currentClip != "bAttack" && player_anim.currentClip != "rAttack")
				return;

			switch (player_data.comboState)
			{
			case 0: // First combo state
				if (player_anim.current_frame == 10)
				{
					// Harvest some soul whenever mochi hits the dummy
					ECS::GetInstance().GetSystem<SoulManager>()->HarvestSoul(FISH, 5.f);
					ECS::GetInstance().GetSystem<SoulManager>()->HarvestSoul(WORM, 5.f);

					ECS::GetInstance().GetSystem<BossManager>()->BossTakeDMG();
					boss_sr.color = Vec3(1.f, 0.f, 0.f);

					if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
					{
						auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
						std::vector<int> DummyHitSounds = {
							audioM.GetSFXindex("DuHurt1"),
							audioM.GetSFXindex("DuHurt2"),
							audioM.GetSFXindex("DuHurt3")
						};

						// Remove invalid (-1) sounds
						DummyHitSounds.erase(
							std::remove(DummyHitSounds.begin(), DummyHitSounds.end(), -1),
							DummyHitSounds.end()
						);

						// Keep track of which sounds have been played (static at class level)
						static std::vector<bool> dummyHitSoundsPlayed;

						// Initialize if first time or size changed
						if (dummyHitSoundsPlayed.size() != DummyHitSounds.size()) {
							dummyHitSoundsPlayed.resize(DummyHitSounds.size(), false);
						}

						// Check if all sounds have been played
						bool allPlayed = true;
						for (bool played : dummyHitSoundsPlayed) {
							if (!played) {
								allPlayed = false;
								break;
							}
						}

						// If all sounds have been played, reset all to unplayed
						if (allPlayed) {
							std::fill(dummyHitSoundsPlayed.begin(), dummyHitSoundsPlayed.end(), false);
						}

						// Get sounds that haven't been played yet
						std::vector<int> availableSoundIndices;
						for (int i = 0; i < DummyHitSounds.size(); i++) {
							if (!dummyHitSoundsPlayed[i]) {
								availableSoundIndices.push_back(i);
							}
						}

						if (!availableSoundIndices.empty()) {
							// Random selection from available sounds
							int randomIndex = rand() % availableSoundIndices.size();
							int selectedSoundIndex = availableSoundIndices[randomIndex];
							int selectedSound = DummyHitSounds[selectedSoundIndex];

							// Mark this sound as played
							dummyHitSoundsPlayed[selectedSoundIndex] = true;

							// Play the selected sound
							audioM.PlaySFX(selectedSound);
						}
					}

					if (player_sr.flipX)
					{
						vfxhit_trans.position = Vector3D(player_trans.position.x + 150.0f, player_trans.position.y, 0);
						vfxhit_anim.RestartAnimation();
					}
					else
					{
						vfxhit_trans.position = Vector3D(player_trans.position.x - 150.0f, player_trans.position.y, 0);
						vfxhit_anim.RestartAnimation();
					}
					player_data.HitStopAnimation();
				}
				else
				{
					boss_sr.color = Vec3(1.f, 1.f, 1.f);
				}
				break;

			case 1: // Second combo state
				if (player_anim.current_frame == 15)
				{
					// Harvest some soul whenever mochi hits the dummy
					ECS::GetInstance().GetSystem<SoulManager>()->HarvestSoul(FISH, 5.f);
					ECS::GetInstance().GetSystem<SoulManager>()->HarvestSoul(WORM, 5.f);
					ECS::GetInstance().GetSystem<BossManager>()->BossTakeDMG();
					boss_sr.color = Vec3(1.f, 0.f, 0.f);

					if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
					{
						auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
						std::vector<int> DummyHitSounds = {
							audioM.GetSFXindex("DuHurt1"),
							audioM.GetSFXindex("DuHurt2"),
							audioM.GetSFXindex("DuHurt3")
						};

						// Remove invalid (-1) sounds
						DummyHitSounds.erase(
							std::remove(DummyHitSounds.begin(), DummyHitSounds.end(), -1),
							DummyHitSounds.end()
						);

						// Keep track of which sounds have been played (static at class level)
						static std::vector<bool> dummyHitSoundsPlayed;

						// Initialize if first time or size changed
						if (dummyHitSoundsPlayed.size() != DummyHitSounds.size()) {
							dummyHitSoundsPlayed.resize(DummyHitSounds.size(), false);
						}

						// Check if all sounds have been played
						bool allPlayed = true;
						for (bool played : dummyHitSoundsPlayed) {
							if (!played) {
								allPlayed = false;
								break;
							}
						}

						// If all sounds have been played, reset all to unplayed
						if (allPlayed) {
							std::fill(dummyHitSoundsPlayed.begin(), dummyHitSoundsPlayed.end(), false);
						}

						// Get sounds that haven't been played yet
						std::vector<int> availableSoundIndices;
						for (int i = 0; i < DummyHitSounds.size(); i++) {
							if (!dummyHitSoundsPlayed[i]) {
								availableSoundIndices.push_back(i);
							}
						}

						if (!availableSoundIndices.empty()) {
							// Random selection from available sounds
							int randomIndex = rand() % availableSoundIndices.size();
							int selectedSoundIndex = availableSoundIndices[randomIndex];
							int selectedSound = DummyHitSounds[selectedSoundIndex];

							// Mark this sound as played
							dummyHitSoundsPlayed[selectedSoundIndex] = true;

							// Play the selected sound
							audioM.PlaySFX(selectedSound);
						}
					}

					if (player_sr.flipX)
					{
						vfxhit_trans.position = Vector3D(player_trans.position.x + 150.0f, player_trans.position.y, 0);
						vfxhit_anim.RestartAnimation();
					}
					else
					{
						vfxhit_trans.position = Vector3D(player_trans.position.x - 150.0f, player_trans.position.y, 0);
						vfxhit_anim.RestartAnimation();
					}
					player_data.HitStopAnimation();
				}
				else
				{
					boss_sr.color = Vec3(1.f, 1.f, 1.f);
				}
				break;

			case 2: // Knockback kick combo
				if (player_anim.current_frame == 31)
				{
					// Harvest some soul whenever mochi hits the dummy
					ECS::GetInstance().GetSystem<SoulManager>()->HarvestSoul(FISH, 5.f);
					ECS::GetInstance().GetSystem<SoulManager>()->HarvestSoul(WORM, 5.f);
					ECS::GetInstance().GetSystem<BossManager>()->BossTakeDMG();
					boss_sr.color = Vec3(1.f, 0.f, 0.f);

					// Play sound effects
					if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
					{
						auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
						if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("Pattack3")))
						{
							audioM.PlaySFX(audioM.GetSFXindex("Pattack3"));
						}
					}

					if (player_sr.flipX)
					{
						vfxhit_trans.position = Vector3D(player_trans.position.x + 150.0f, player_trans.position.y, 0);
						vfxhit_anim.RestartAnimation();
					}
					else
					{
						vfxhit_trans.position = Vector3D(player_trans.position.x - 150.0f, player_trans.position.y, 0);
						vfxhit_anim.RestartAnimation();
					}
					player_data.HitStopAnimation();
				}
				else
				{
					boss_sr.color = Vec3(1.f, 1.f, 1.f);
				}
				break;

			default:
				break;
			}
			}
		else if (tag1 == "Knife" && tag2 == "Dummy") // Mochi's Knife and Dummy
		{
			// Get references of the player and enemy
			auto& player_trans = ECS::GetInstance().GetComponent<Transform>(player);
			auto& player_sr = ECS::GetInstance().GetComponent<SpriteRender>(player);
			auto& player_data = ECS::GetInstance().GetComponent<Player>(player);
			auto& player_anim = ECS::GetInstance().GetComponent<Animation>(player);
			auto& dummy_anim = ECS::GetInstance().GetComponent<Animation>(entity2);
			auto& vfxhit_trans = GameObjectManager::GetInstance().GetGOByName("Hit_Effect")->GetComponent<Transform>();
			auto& vfxhit_anim = GameObjectManager::GetInstance().GetGOByName("Hit_Effect")->GetComponent<Animation>();

			// Skip if Mochi is not attacking
			if (player_anim.currentClip != "Attack" && player_anim.currentClip != "bAttack" && player_anim.currentClip != "rAttack")
				return;

			switch (player_data.comboState)
			{
			case 0: // First combo state
				if (player_anim.current_frame == 10)
				{
					// Harvest some soul whenever mochi hits the dummy
					ECS::GetInstance().GetSystem<SoulManager>()->HarvestSoul(FISH, 5.f);
					ECS::GetInstance().GetSystem<SoulManager>()->HarvestSoul(WORM, 5.f);

					if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
					{
						auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
						std::vector<int> DummyHitSounds = {
							audioM.GetSFXindex("DuHurt1"),
							audioM.GetSFXindex("DuHurt2"),
							audioM.GetSFXindex("DuHurt3")
						};

						// Remove invalid (-1) sounds
						DummyHitSounds.erase(
							std::remove(DummyHitSounds.begin(), DummyHitSounds.end(), -1),
							DummyHitSounds.end()
						);

						// Keep track of which sounds have been played (static at class level)
						static std::vector<bool> dummyHitSoundsPlayed;

						// Initialize if first time or size changed
						if (dummyHitSoundsPlayed.size() != DummyHitSounds.size()) {
							dummyHitSoundsPlayed.resize(DummyHitSounds.size(), false);
						}

						// Check if all sounds have been played
						bool allPlayed = true;
						for (bool played : dummyHitSoundsPlayed) {
							if (!played) {
								allPlayed = false;
								break;
							}
						}

						// If all sounds have been played, reset all to unplayed
						if (allPlayed) {
							std::fill(dummyHitSoundsPlayed.begin(), dummyHitSoundsPlayed.end(), false);
						}

						// Get sounds that haven't been played yet
						std::vector<int> availableSoundIndices;
						for (int i = 0; i < DummyHitSounds.size(); i++) {
							if (!dummyHitSoundsPlayed[i]) {
								availableSoundIndices.push_back(i);
							}
						}

						if (!availableSoundIndices.empty()) {
							// Random selection from available sounds
							int randomIndex = rand() % availableSoundIndices.size();
							int selectedSoundIndex = availableSoundIndices[randomIndex];
							int selectedSound = DummyHitSounds[selectedSoundIndex];

							// Mark this sound as played
							dummyHitSoundsPlayed[selectedSoundIndex] = true;

							// Play the selected sound
							audioM.PlaySFX(selectedSound);
						}
					}

					if (player_sr.flipX)
					{
						vfxhit_trans.position = Vector3D(player_trans.position.x + 150.0f, player_trans.position.y, 0);
						vfxhit_anim.RestartAnimation();
					}
					else
					{
						vfxhit_trans.position = Vector3D(player_trans.position.x - 150.0f, player_trans.position.y, 0);
						vfxhit_anim.RestartAnimation();
					}
					dummy_anim.RestartAnimation();
					player_data.HitStopAnimation();
				}
				break;

			case 1: // Second combo state
				if (player_anim.current_frame == 15)
				{
					// Harvest some soul whenever mochi hits the dummy
					ECS::GetInstance().GetSystem<SoulManager>()->HarvestSoul(FISH, 5.f);
					ECS::GetInstance().GetSystem<SoulManager>()->HarvestSoul(WORM, 5.f);

					if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
					{
						auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
						std::vector<int> DummyHitSounds = {
							audioM.GetSFXindex("DuHurt1"),
							audioM.GetSFXindex("DuHurt2"),
							audioM.GetSFXindex("DuHurt3")
						};

						// Remove invalid (-1) sounds
						DummyHitSounds.erase(
							std::remove(DummyHitSounds.begin(), DummyHitSounds.end(), -1),
							DummyHitSounds.end()
						);

						// Keep track of which sounds have been played (static at class level)
						static std::vector<bool> dummyHitSoundsPlayed;

						// Initialize if first time or size changed
						if (dummyHitSoundsPlayed.size() != DummyHitSounds.size()) {
							dummyHitSoundsPlayed.resize(DummyHitSounds.size(), false);
						}

						// Check if all sounds have been played
						bool allPlayed = true;
						for (bool played : dummyHitSoundsPlayed) {
							if (!played) {
								allPlayed = false;
								break;
							}
						}

						// If all sounds have been played, reset all to unplayed
						if (allPlayed) {
							std::fill(dummyHitSoundsPlayed.begin(), dummyHitSoundsPlayed.end(), false);
						}

						// Get sounds that haven't been played yet
						std::vector<int> availableSoundIndices;
						for (int i = 0; i < DummyHitSounds.size(); i++) {
							if (!dummyHitSoundsPlayed[i]) {
								availableSoundIndices.push_back(i);
							}
						}

						if (!availableSoundIndices.empty()) {
							// Random selection from available sounds
							int randomIndex = rand() % availableSoundIndices.size();
							int selectedSoundIndex = availableSoundIndices[randomIndex];
							int selectedSound = DummyHitSounds[selectedSoundIndex];

							// Mark this sound as played
							dummyHitSoundsPlayed[selectedSoundIndex] = true;

							// Play the selected sound
							audioM.PlaySFX(selectedSound);
						}
					}

					if (player_sr.flipX)
					{
						vfxhit_trans.position = Vector3D(player_trans.position.x + 150.0f, player_trans.position.y, 0);
						vfxhit_anim.RestartAnimation();
					}
					else
					{
						vfxhit_trans.position = Vector3D(player_trans.position.x - 150.0f, player_trans.position.y, 0);
						vfxhit_anim.RestartAnimation();
					}
					dummy_anim.RestartAnimation();
					player_data.HitStopAnimation();
				}
				break;

			case 2: // Knockback kick combo
				if (player_anim.current_frame == 31)
				{
					// Harvest some soul whenever mochi hits the dummy
					ECS::GetInstance().GetSystem<SoulManager>()->HarvestSoul(FISH, 5.f);
					ECS::GetInstance().GetSystem<SoulManager>()->HarvestSoul(WORM, 5.f);

					// Play sound effects
					if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
					{
						auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
						if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsSFXPlaying(audioM.GetSFXindex("Pattack3")))
						{
							audioM.PlaySFX(audioM.GetSFXindex("Pattack3"));
						}
					}

					if (player_sr.flipX)
					{
						vfxhit_trans.position = Vector3D(player_trans.position.x + 150.0f, player_trans.position.y, 0);
						vfxhit_anim.RestartAnimation();
					}
					else
					{
						vfxhit_trans.position = Vector3D(player_trans.position.x - 150.0f, player_trans.position.y, 0);
						vfxhit_anim.RestartAnimation();
					}
					dummy_anim.RestartAnimation();
					player_data.HitStopAnimation();
				}
				break;

			default:
				break;
			}
		}

		// Skip trigger objects
		if (box1.is_trigger || box2.is_trigger)
			return;

		if (tag1 == "Player" && tag2 == "Enemy" || tag1 == "Player" && tag2 == "EnemyProjectile") // Mochi and Enemy / Enemy's Projectile (Mochi takes damage)
		{
			// Get references of the player and enemy
			auto& player_data = ECS::GetInstance().GetComponent<Player>(player);
			auto& player_trans = ECS::GetInstance().GetComponent<Transform>(player);
			auto& vfx_trans = GameObjectManager::GetInstance().GetGOByName("Projectile_Hit_Effect")->GetComponent<Transform>();
			auto& vfx_sr = GameObjectManager::GetInstance().GetGOByName("Projectile_Hit_Effect")->GetComponent<SpriteRender>();
			auto& vfx_anim = GameObjectManager::GetInstance().GetGOByName("Projectile_Hit_Effect")->GetComponent<Animation>();

			if (tag2 == "Enemy")
			{
				// Deal damage to the player
			}
			else if (tag2 == "EnemyProjectile" && ECS::GetInstance().HasComponent<EnemyBullet>(entity2))
			{
				auto& bullet_data = ECS::GetInstance().GetComponent<EnemyBullet>(entity2);
				auto& bullet_trans = ECS::GetInstance().GetComponent<Transform>(entity2);

				if (!bullet_data.hit)
				{
					if (!player_data.isDead)
					{
						ECS::GetInstance().GetSystem<PlayerManager>()->OnCollisionEnter(player);
						if (bullet_trans.position.x < player_trans.position.x)
						{
							vfx_trans.position = Vector3D(player_trans.position.x - 80.0f, player_trans.position.y, 0);
							vfx_sr.flipX = true;
						}
						else if (bullet_trans.position.x > player_trans.position.x)
						{
							vfx_trans.position = Vector3D(player_trans.position.x + 80.0f, player_trans.position.y, 0);
							vfx_sr.flipX = false;
						}

						vfx_anim.RestartAnimation();

						if (!SceneManager::GetInstance().enable_cheatmode) // Dont take dmg if cheat mode is enabled
							player_data.currentHealth -= 10;
					}
					bullet_data.hit = true;
				}
			}
		}
		else if (tag1 == "Player" && tag2 == "Environment" || tag1 == "Player" && tag2 == "Dummy" || tag1 == "Player" && tag2 == "Boundary") // Mochi and Environment Objects / Dummy / Boundaries (Acts as a wall)
		{
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
		else if (tag1 == "Enemy" && tag2 == "Environment" || tag1 == "Enemy" && tag2 == "Boundary") // Enemy and Environment Objects / Boundaries (Acts as a wall)
		{
			ECS::GetInstance().GetSystem<EnemyManager>()->EnemyCollisionResponse(entity1, entity2);

			// STATIC AND DYNAMIC
			Static_Response(trans1, box1, rb1, trans2, box2, rb2);
			StaticDynamic_Response(trans1, box1, rb1, trans2, box2, rb2, firstTimeOfCollision);
		}
		//else if (tag1 == "Enemy" && tag2 == "Enemy") // Enemy and Enemy (Block each other)
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
		// Box 1 left and box 2 right collision response
		if (box1.collision_flag & COLLISION_LEFT && box2.collision_flag & COLLISION_RIGHT)
		{
			float overlap = abs(box2.max.x - box1.min.x); // Calculate overlap depth
			if (!rb1.is_kinematic)
				trans1.position.x += overlap + MIN_OFFSET; // Move box1 out of the collision
			if (!rb2.is_kinematic)
				trans2.position.x -= overlap + MIN_OFFSET; // Move box2 out of the collision
		}

		// Box 1 right and box 2 left collision response
		if (box1.collision_flag & COLLISION_RIGHT && box2.collision_flag & COLLISION_LEFT)
		{
			float overlap = abs(box1.max.x - box2.min.x); // Calculate overlap depth
			if (!rb1.is_kinematic)
				trans1.position.x -= overlap + MIN_OFFSET; // Move box1 out of the collision
			if (!rb2.is_kinematic)
				trans2.position.x += overlap + MIN_OFFSET; // Move box2 out of the collision
		}

		// Box 1 top and box 2 bottom collision response
		if (box1.collision_flag & COLLISION_TOP && box2.collision_flag & COLLISION_BOTTOM)
		{
			float overlap = abs(box2.max.y - box1.min.y); // Calculate overlap depth
			if (!rb1.is_kinematic)
				trans1.position.y += overlap + MIN_OFFSET; // Move box1 out of the collision
			if (!rb2.is_kinematic)
				trans2.position.y -= overlap + MIN_OFFSET; // Move box2 out of the collision
		}

		// Box 1 bottom and box 2 top collision response
		if (box1.collision_flag & COLLISION_BOTTOM && box2.collision_flag & COLLISION_TOP)
		{
			float overlap = abs(box1.max.y - box2.min.y); // Calculate overlap depth
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