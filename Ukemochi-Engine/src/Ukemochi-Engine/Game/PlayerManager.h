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
#include "Ukemochi-Engine/ECS/Components.h"
#include "Ukemochi-Engine/ECS/Systems.h"

namespace Ukemochi
{
    class PlayerManager : public System
    {
        /**
         * @biref Handle player movement 
         * @param rb player's rigidbody
         * @param anim player's animation
         * @param sr player's spriteRenderer
         * @param data player's variables
         */
        void PlayersMovement(Rigidbody2D& rb, Animation& anim, SpriteRender& sr, const Player& data) const;
        
        /**
         * @brief Handle player's animation depending on the soul(color) it is on
         * @param soulData player's soulData
         * @param clip the clip that's gonna to be played
         * @return the clip with marked soul flag
         */
        std::string SoulAnimation(const PlayerSoul& soulData, std::string clip) const;

        /**
         * @brief Handle if player animation is idle, depending on the current soul
         * @param anim player's animation
         * @param soulData player's soulData
         * @param clip the clip that's gonna to be played
         * @return true if animation is idle, not otherwise
         */
        bool CheckIfIdle(const Animation& anim) const;

    public:
        /**
         * @brief Update the player manager
         */
        void Update() const;

        /**
         * @brief Handles the collision event when the player collides with another object.
         * @param id The entity ID of the object that collided with the player.
         */
        void OnCollisionEnter(const EntityID& id) const;
    };
}
