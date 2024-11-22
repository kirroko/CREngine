#include "PreCompile.h"
#include "Animation.h"

#include "GLFW/glfw3.h"
#include "Ukemochi-Engine/ECS/ECS.h"

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
            auto& animation = ECS::GetInstance().GetComponent<Animation>(entity);

            GLfloat currentFrameTime = static_cast<GLfloat>(glfwGetTime());
            float deltaTime = currentFrameTime - m_time;
            m_time = currentFrameTime;
            
            animation.update(deltaTime);
        }
    }
}
