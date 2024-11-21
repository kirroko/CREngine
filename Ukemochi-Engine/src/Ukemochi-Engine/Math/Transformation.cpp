/* Start Header ************************************************************************/
/*!
\file       Transformation.cpp
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Nov 19, 2024
\brief      This file contains the definition of the Transformation system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "Transformation.h"				  // for forward declaration
#include "../Math/MathUtils.h"			  // for radian, clamp
#include "../FrameController.h"			  // for delta time
#include "../Factory/GameObjectManager.h" // for player game object

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

			// If the entity is the player, adjust based on the direction and scaling factor
			if (GameObjectManager::GetInstance().GetGO(entity)->GetTag() == "Player")
				scale.m2[0][0] = isFacingRight ? -scale.m2[0][0] : scale.m2[0][0]; // Adjust X-axis scale to flip direction if not facing right

			transform.transform_matrix = trans * rot * scale;

			// Compute the depth scale of the dynamic entities
			if (GameObjectManager::GetInstance().GetGO(entity)->GetTag() == "Player"
				|| GameObjectManager::GetInstance().GetGO(entity)->GetTag() == "Enemy")
				ComputeObjectScale(entity, OBJECT_SCALING);
		}
	}

	/*!***********************************************************************
	\brief
	 Compute the scale of a object based on its y-axis position.
	\param[out] object
	 The object to scale.
	\param[in] scaling
	 The DepthScaling range.
	*************************************************************************/
	void Transformation::ComputeObjectScale(EntityID object, const DepthScaling& scaling)
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

		// Set the new scale
		transform.scale = { new_scale, new_scale };
	}

	void Transformation::IncreaseScale(Transform& trans)
	{
		trans.scale += Vec2{ SCALE_FACTOR, SCALE_FACTOR } * static_cast<float>(g_FrameRateController.GetDeltaTime());
		trans.scale.x = clamp(trans.scale.x, MIN_SCALE, MAX_SCALE);
		trans.scale.y = clamp(trans.scale.y, MIN_SCALE, MAX_SCALE);
	}

	void Transformation::DecreaseScale(Transform& trans)
	{
		trans.scale -= Vec2{ SCALE_FACTOR, SCALE_FACTOR } * static_cast<float>(g_FrameRateController.GetDeltaTime());
		trans.scale.x = clamp(trans.scale.x, MIN_SCALE, MAX_SCALE);
		trans.scale.y = clamp(trans.scale.y, MIN_SCALE, MAX_SCALE);
	}
}