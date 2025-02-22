/* Start Header ************************************************************************/
/*!
\file       Transformation.cpp
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Feb 21, 2025
\brief      This file contains the definition of the Transformation system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "Transformation.h"				  // for forward declaration
#include "../Math/MathUtils.h"			  // for radian, clamp
#include "../FrameController.h"			  // for fixed delta time
#include "../Factory/GameObjectManager.h" // for game object tag

namespace Ukemochi
{
	/*!***********************************************************************
	\brief
	 Compute the transformations of all the entities.
	*************************************************************************/
	void Transformation::ComputeTransformations()
	{
		for (auto& entity : m_Entities)
		{
			// Set up the entity matrix using the transform's position, rotation and scale
			auto& transform = ECS::GetInstance().GetComponent<Transform>(entity);

			Mtx44 trans{}, rot{}, scale{};
			Mtx44Translate(trans, transform.position.x, transform.position.y, 0);
			Mtx44RotZRad(rot, radian(transform.rotation));
			Mtx44Scale(scale, transform.scale.x, transform.scale.y, 0);

			transform.transform_matrix = trans * rot * scale;

			// Skip if the entity is not active
			if (!GameObjectManager::GetInstance().GetGO(entity)->GetActive())
				continue;

			// Get the tag of the entity
			std::string tag = GameObjectManager::GetInstance().GetGO(entity)->GetTag();

			// Compute the depth scale of the dynamic entities
			if (tag == "Player" || tag == "Knife" || tag == "Soul" || tag == "FishAbility" || tag == "WormAbility" || tag == "Enemy")
				ComputeObjectScale(entity, OBJECT_SCALING);

			// Compute the layer of the dynamic entities
			if (tag == "Player" || tag == "Soul" || tag == "FishAbility" || tag == "WormAbility" || tag == "Enemy")
				ComputeObjectLayer(entity);
		}
	}

	/*!***********************************************************************
	\brief
	 Compute the scale of a object based on its y-axis position.
	\param[in/out] object
	 The object to scale.
	\param[in] scaling
	 The DepthScaling range.
	*************************************************************************/
	void Transformation::ComputeObjectScale(const EntityID& object, const DepthScaling& scaling)
	{
		// Get the object's transform component
		auto& transform = ECS::GetInstance().GetComponent<Transform>(object);

		// Clamp the object's Y position within the bounds
		float clamp_y = clamp(transform.position.y, scaling.min_y, scaling.max_y);

		// Compute the interpolation factor from (0 to 1)
		float t = (clamp_y - scaling.min_y) / (scaling.max_y - scaling.min_y);

		// Reverse the interpolation for flipping
		float flip_t = 1.0f - t;

		// Interpolate the scale
		float new_scale = scaling.min_scale + flip_t * (scaling.max_scale - scaling.min_scale);

		// Get the tag of the object
		std::string tag = GameObjectManager::GetInstance().GetGO(object)->GetTag();

		// Set the new scale
		if (tag == "Knife")
			transform.scale = { new_scale * 0.5f, new_scale * 0.75f };
		else if (tag == "Soul")
			transform.scale = { new_scale * 0.1f, new_scale * 0.1f };
		else if (tag == "FishAbility" || tag == "WormAbility")
			transform.scale = { new_scale * 2.f, new_scale * 2.f };
		else
			transform.scale = { new_scale, new_scale };
	}

	/*!***********************************************************************
	\brief
	 Compute the layer of a object based on its y-axis position.
	\param[in/out] object
	 The object to compute the layer for.
	*************************************************************************/
	void Transformation::ComputeObjectLayer(const EntityID& object)
	{
		auto& transform = ECS::GetInstance().GetComponent<Transform>(object);
		auto& sprite_renderer = ECS::GetInstance().GetComponent<SpriteRender>(object);

		bool is_behind = false;

		Vec3 object_position = transform.position;
		Vec3 nearest_static_position{};
		float min_distance = std::numeric_limits<float>::max();

		// Search through the entity list for the nearest static object
		for (auto const& static_entity : m_Entities)
		{
			if (GameObjectManager::GetInstance().GetGO(static_entity)->GetTag() == "Environment"
				&& GameObjectManager::GetInstance().GetGO(static_entity)->GetActive())
			{
				Vec3 static_position = ECS::GetInstance().GetComponent<Transform>(static_entity).position;
				float distance = Vec3Length(static_position - object_position);

				if (distance < min_distance)
				{
					min_distance = distance;
					nearest_static_position = static_position;
				}
			}
		}

		// Compare the Y positions of the dynamic object and the nearest static object
		if (object_position.y > nearest_static_position.y)
			is_behind = true;

		// Get the tag of the object
		std::string tag = GameObjectManager::GetInstance().GetGO(object)->GetTag();

		// Set the layer based on whether the object is behind or infront the static objects
		if (tag == "Player" || tag == "Soul" || tag == "Enemy")
			sprite_renderer.layer = is_behind ? DYNAMIC_BACK : DYNAMIC_FRONT;
		else if (tag == "FishAbility" || tag == "WormAbility")
			sprite_renderer.layer = is_behind ? SKILL_BACK : SKILL_FRONT;
	}

	/*!***********************************************************************
	\brief
	 Increase the scale of the object.
	\param[out] trans
	 The transform component to scale.
	*************************************************************************/
	void Transformation::IncreaseScale(Transform& trans)
	{
		trans.scale += Vec2{ SCALE_FACTOR, SCALE_FACTOR } * static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
		trans.scale.x = clamp(trans.scale.x, MIN_SCALE, MAX_SCALE);
		trans.scale.y = clamp(trans.scale.y, MIN_SCALE, MAX_SCALE);
	}

	/*!***********************************************************************
	\brief
	 Decrease the scale of the object.
	\param[out] trans
	 The transform component to scale.
	*************************************************************************/
	void Transformation::DecreaseScale(Transform& trans)
	{
		trans.scale -= Vec2{ SCALE_FACTOR, SCALE_FACTOR } * static_cast<float>(g_FrameRateController.GetFixedDeltaTime());
		trans.scale.x = clamp(trans.scale.x, MIN_SCALE, MAX_SCALE);
		trans.scale.y = clamp(trans.scale.y, MIN_SCALE, MAX_SCALE);
	}
}