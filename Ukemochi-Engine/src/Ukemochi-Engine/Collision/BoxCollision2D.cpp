/* Start Header
*****************************************************************/
/*!
\file       BoxCollision2D.cpp
\author     Lum Ko Sand, kosand.lum, 2301263
\par        email: kosand.lum\@digipen.edu
\date       Sept 11, 2024
\brief      This file contains the definition of the BoxCollision2D system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "PreCompile.h"
#include "BoxCollision2D.h"
#include "../Math/MathUtils.h" // for min, max

namespace Ukemochi
{
	/*!***********************************************************************
	\brief
	 Implementation of collision detection between two rectangles.
	\param[in] aabb1
	 The bounding box of the first rectangle.
	\param[in] vel1
	 The velocity of the first rectangle.
	\param[in] aabb2
	 The bounding box of the second rectangle.
	\param[in] vel2
	 The velocity of the second rectangle.
	\param[out] firstTimeOfCollision
	 Output variable for the calculated "tFirst" value.
	\return
	 True if there is an intersection, false otherwise.
	*************************************************************************/
	bool CollisionIntersection_RectRect(const AABB &aabb1,
										const Vec2 &vel1,
										const AABB &aabb2,
										const Vec2 &vel2,
										float &firstTimeOfCollision)
	{
		// Step 1: Check for static collision detection between rectangles
		if (aabb1.max.x < aabb2.min.x || aabb1.min.x > aabb2.max.x ||
			aabb1.max.y < aabb2.min.y || aabb1.min.y > aabb2.max.y)
		{
			// No overlapping rectangles
			// return 0;

			// Step 2: Initialize and calculate the new velocity of Vb
			firstTimeOfCollision = 0.0f;
			float tLast = 0;// (float)AEFrameRateControllerGetFrameTime(); // NEED ACCESS TO FRAME TIME!!
			Vec2 vRel = {vel2.x - vel1.x, vel2.y - vel1.y};

			// Step 3: Working with one dimension (x-axis)
			if (vRel.x < 0)
			{
				// Case 1
				if (aabb1.min.x > aabb2.max.x)
					return 0; // No intersection and B is moving away

				// Case 4
				if (aabb1.max.x < aabb2.min.x)
					firstTimeOfCollision = max((aabb1.max.x - aabb2.min.x) / vRel.x, firstTimeOfCollision);
				if (aabb1.min.x < aabb2.max.x)
					tLast = min((aabb1.min.x - aabb2.max.x) / vRel.x, tLast);
			}
			else if (vRel.x > 0)
			{
				// Case 2
				if (aabb1.min.x > aabb2.max.x)
					firstTimeOfCollision = max((aabb1.min.x - aabb2.max.x) / vRel.x, firstTimeOfCollision);
				if (aabb1.max.x > aabb2.min.x)
					tLast = min((aabb1.max.x - aabb2.min.x) / vRel.x, tLast);

				// Case 3
				if (aabb1.max.x < aabb2.min.x)
					return 0; // No intersection and B is moving away
			}
			else if (vRel.x == 0)
			{
				// Case 5
				if (aabb1.max.x < aabb2.min.x || aabb1.min.x > aabb2.max.x)
					return 0; // B is moving parallel to the opposite coordinate axis
			}

			// Case 6
			if (firstTimeOfCollision > tLast)
				return 0; // No intersection

			// Step 4: Working with one dimension (y-axis)
			if (vRel.y < 0)
			{
				// Case 1
				if (aabb1.min.y > aabb2.max.y)
					return 0; // No intersection and B is moving away

				// Case 4
				if (aabb1.max.y < aabb2.min.y)
					firstTimeOfCollision = max((aabb1.max.y - aabb2.min.y) / vRel.y, firstTimeOfCollision);
				if (aabb1.min.y < aabb2.max.y)
					tLast = min((aabb1.min.y - aabb2.max.y) / vRel.y, tLast);
			}
			else if (vRel.y > 0)
			{
				// Case 2
				if (aabb1.min.y > aabb2.max.y)
					firstTimeOfCollision = max((aabb1.min.y - aabb2.max.y) / vRel.y, firstTimeOfCollision);
				if (aabb1.max.y > aabb2.min.y)
					tLast = min((aabb1.max.y - aabb2.min.y) / vRel.y, tLast);

				// Case 3
				if (aabb1.max.y < aabb2.min.y)
					return 0; // No intersection and B is moving away
			}
			else if (vRel.y == 0)
			{
				// Case 5
				if (aabb1.max.y < aabb2.min.y || aabb1.min.y > aabb2.max.y)
					return 0; // B is moving parallel to the opposite coordinate axis
			}

			// Case 6
			if (firstTimeOfCollision > tLast)
				return 0; // No intersection

			// Step 5: Return true: the rectangles intersect
			return 1;
		}
		else
		{
			// Overlapping rectangles
			return 1;
		}
	}
}