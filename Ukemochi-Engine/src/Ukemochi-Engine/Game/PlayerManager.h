/* Start Header ************************************************************************/
/*!
\file       PlayerManager.h
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date       Nov 21, 2024
\brief      This file is a temporary solution to manage player's logic and should replace to the C# script as soon as possible. 

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#pragma once
#include "Ukemochi-Engine/ECS/Systems.h"

namespace Ukemochi
{
    class PlayerManager : public System
    {
    public:
        void Update() const;

        /**
         * @brief Handles the collision event when the player collides with another object.
         * @param id The entity ID of the object that collided with the player.
         */
        void OnCollisionEnter(const EntityID& id) const;
    };
}
