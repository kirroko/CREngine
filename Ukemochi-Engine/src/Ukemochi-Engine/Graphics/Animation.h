/* Start Header ************************************************************************/
/*!
\file       Animation.h
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date       Nov 16, 2024
\brief      This file handles Updating the animation for the entities.


Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#pragma once
#include "Ukemochi-Engine/ECS/Systems.h"

namespace Ukemochi
{
    class AnimationSystem : public System
    {
    public:
        /**
         * @brief Initialize the Animation System
         */
        void Init();
        /**
         * @brief Update the Animation System
         */
        void Update() const;
    };
}
