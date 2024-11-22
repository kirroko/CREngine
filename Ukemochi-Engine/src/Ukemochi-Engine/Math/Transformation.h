/* Start Header ************************************************************************/
/*!
\file       Transformation.h
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Nov 19, 2024
\brief      This file contains the declaration of the Transformation system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include "../ECS/ECS.h" // for ECS system and components

namespace Ukemochi
{
    /*!***********************************************************************
    \brief
     Scale a object based on its y-axis position to create a pseudo 3D depth effect.
    *************************************************************************/
    struct DepthScaling
    {
        float min_y;     // Minimum y-axis position
        float max_y;     // Maximum y-axis position
        float min_scale; // Minimum scale at max y-axis position
        float max_scale; // Maximum scale at min y-axis position
    };

    class Transformation : public System
	{
	public:
        /*!***********************************************************************
        \brief
         Minimum and maximum scale constant for all entities.
        *************************************************************************/
        const float MIN_Y_POS = 0.0f, MAX_Y_POS = 900.0f;
        const float MIN_SCALE = 100.0f, MAX_SCALE = 175.0f;
        const float SCALE_FACTOR = 100.f;

        const DepthScaling OBJECT_SCALING = { MIN_Y_POS, MAX_Y_POS, MIN_SCALE, MAX_SCALE };

        bool isFacingRight{ false };

        /*!***********************************************************************
        \brief
         Compute the transformations of all the entities.
        *************************************************************************/
		void ComputeTransformations();

        /*!***********************************************************************
        \brief
         Compute the scale of a object based on its y-axis position.
        \param[out] object
         The object to scale.
        \param[in] scaling
         The DepthScaling range.
        *************************************************************************/
        void ComputeObjectScale(EntityID object, const DepthScaling& scaling);

        void IncreaseScale(Transform& trans); // Increase the scale of the object
        void DecreaseScale(Transform& trans); // Decrease the scale of the object
	};
}