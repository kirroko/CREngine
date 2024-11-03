/* Start Header
*****************************************************************/
/*!
\file       Transformation.cpp
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\co-author  TAN Shun Zhi Tomy, t.shunzhitomy, , t.shunzhitomy@digipen.edu
\date       Oct 31, 2024
\brief      This file contains the definition of the Transformation system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "PreCompile.h"
#include "Transformation.h"	    // for forward declaration
#include "../Math/MathUtils.h"  // for radian, clamp
#include "../FrameController.h" // for delta time

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
		}
	}

	void Transformation::IncreaseScale(Transform& trans)
	{
		trans.scale += Vec2{ SCALE_FACTOR, SCALE_FACTOR } * g_FrameRateController.GetDeltaTime();
		trans.scale.x = clamp(trans.scale.x, MIN_SCALE, MAX_SCALE);
		trans.scale.y = clamp(trans.scale.y, MIN_SCALE, MAX_SCALE);
	}

	void Transformation::DecreaseScale(Transform& trans)
	{
		trans.scale -= Vec2{ SCALE_FACTOR, SCALE_FACTOR } * g_FrameRateController.GetDeltaTime();
		trans.scale.x = clamp(trans.scale.x, MIN_SCALE, MAX_SCALE);
		trans.scale.y = clamp(trans.scale.y, MIN_SCALE, MAX_SCALE);
	}
}