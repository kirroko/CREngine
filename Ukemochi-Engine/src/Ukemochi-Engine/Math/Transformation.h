/* Start Header
*****************************************************************/
/*!
\file       Transformation.h
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\co-author  TAN Shun Zhi Tomy, t.shunzhitomy, , t.shunzhitomy@digipen.edu
\date       Oct 31, 2024
\brief      This file contains the declaration of the Transformation system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#pragma once

#include "../ECS/ECS.h" // for ECS system and components

namespace Ukemochi
{
    /*!***********************************************************************
    \brief
     Minimum and maximum scale constant for all entities.
    *************************************************************************/
    const float MIN_SCALE = 50.0f, MAX_SCALE = 150.0f;
    const float SCALE_FACTOR = 100.f;

    class Transformation : public System
	{
	public:
        /*!***********************************************************************
        \brief
         Compute the transformations of all the entities.
        *************************************************************************/
		void ComputeTransformations();

        void IncreaseScale(Transform& trans); // Increase the scale of the object
        void DecreaseScale(Transform& trans); // Decrease the scale of the object
	};
}