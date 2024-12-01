/* Start Header ************************************************************************/
/*!
\file       Animation.cpp
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date       Nov 16, 2024
\brief      This file handles Updating the animation for the entities.


Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#include "PreCompile.h"
#include "Animation.h"

#include "Ukemochi-Engine/ECS/ECS.h"
#include "Ukemochi-Engine/Factory/GameObjectManager.h"
#include "../FrameController.h"

namespace Ukemochi
{
    /**
     * @brief Initialize the Animation System
     */
    void AnimationSystem::Init()
    {
        // Read for any metaData of the texture and set the animation clips
        // for(auto& entity : m_Entities)
        // {
        //     auto& animation = ECS::GetInstance().GetComponent<Animation>(entity);
        // }
    }

    /**
     * @brief Update the Animation System
     */
    void AnimationSystem::Update() const
    {
        for(auto& entity : m_Entities)
        {
            if (!GameObjectManager::GetInstance().GetGO(entity)->GetActive())
                continue;
            
            auto& animation = ECS::GetInstance().GetComponent<Animation>(entity);

            float dt = static_cast<float>(g_FrameRateController.GetDeltaTime());
            
            animation.update(dt);
        }
    }
}
