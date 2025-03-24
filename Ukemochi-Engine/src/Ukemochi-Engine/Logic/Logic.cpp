/* Start Header ************************************************************************/
/*!
\file       Logic.cpp
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date       Oct 22, 2024
\brief      This files implements the logic system that handles scripting of each entity within the game.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "Logic.h"
#include "../ECS/ECS.h"

namespace Ukemochi
{
    /**
     * @brief Constructor for the Logic System
     */
    LogicSystem::LogicSystem()
    {
        // ScriptingEngine::GetInstance().Init();
    }

    /**
     * @brief Initialize the Logic System
     */
    void LogicSystem::Init() const
    {
        for (auto& entity : m_Entities)
        {
            auto& logic = ECS::GetInstance().GetComponent<Script>(entity);

            ScriptingEngine::InvokeMethod(ScriptingEngine::GetObjectFromGCHandle(logic.handle), "Start", false);
        }
    }

    /**
     * @brief Update the Logic System
     */
    void LogicSystem::Update() const
    {
        for (auto& entity : m_Entities)
        {
            auto& logic = ECS::GetInstance().GetComponent<Script>(entity);

            ScriptingEngine::InvokeMethod(ScriptingEngine::GetObjectFromGCHandle(logic.handle), "Update", false);
        }
    }
}
