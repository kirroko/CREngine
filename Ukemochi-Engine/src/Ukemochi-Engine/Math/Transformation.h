/* Start Header ************************************************************************/
/*!
\file       Transformation.h
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Mar 23, 2025
\brief      This file contains the declaration of the Transformation system.

Copyright (C) 2025 DigiPen Institute of Technology.
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
    private:
        /*!***********************************************************************
        \brief
         Minimum and maximum Y position and scale constant for all entities.
        *************************************************************************/
        const float MIN_Y_POS = 100.0f, MAX_Y_POS = 800.0f;
        const float MIN_SCALE = 150.0f, MAX_SCALE = 250.0f;

        /*!***********************************************************************
        \brief
         Depth object scaling constant for the entities.
        *************************************************************************/
        const Vec2 KNIFE_SCALE_FACTOR{ 0.5f, 0.75f };
        const Vec2 SOUL_SCALE_FACTOR{ 0.1f, 0.1f };
        const Vec2 SKILL_SCALE_FACTOR{ 2.f, 2.f };
        const Vec2 PROJECTILE_SCALE_FACTOR{ 0.15f, 0.15f };
        const DepthScaling OBJECT_SCALING{ MIN_Y_POS, MAX_Y_POS, MIN_SCALE, MAX_SCALE }; // Object scaling constant for the depth effect.

    public:
        EntityID soul;   // Keep track of the floating soul ID
        EntityID shadow; // Keep track of the player's shadow ID

        /*!***********************************************************************
        \brief
         Initialize the transformation system.
        *************************************************************************/
        void Init();

        /*!***********************************************************************
        \brief
         Compute the transformations of all the entities.
        *************************************************************************/
        void ComputeTransformations();

    private:
        /*!***********************************************************************
        \brief
         Compute the scale of a object based on its y-axis position.
        \param[in/out] object
         The object to scale.
        \param[in] scaling
         The DepthScaling range.
        *************************************************************************/
        void ComputeObjectScale(const EntityID& object, const DepthScaling& scaling);

        /*!***********************************************************************
        \brief
         Compute the layer of a object based on its y-axis position.
        \param[in/out] object
         The object to compute the layer for.
        *************************************************************************/
        void ComputeObjectLayer(const EntityID& object);
    };
}