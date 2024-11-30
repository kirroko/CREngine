#include "PreCompile.h"
#include "Animation.h"

#include "GLFW/glfw3.h"
#include "Ukemochi-Engine/ECS/ECS.h"
#include "Ukemochi-Engine/Factory/GameObjectManager.h"

namespace Ukemochi
{
    void AnimationSystem::Init()
    {
        // Read for any metaData of the texture and set the animation clips
        // for(auto& entity : m_Entities)
        // {
        //     auto& animation = ECS::GetInstance().GetComponent<Animation>(entity);
        // }
    }

    void AnimationSystem::Update()
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
