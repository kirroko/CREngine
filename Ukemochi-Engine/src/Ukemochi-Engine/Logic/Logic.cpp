/* Start Header
*****************************************************************/
/*!
\file	Logic.cpp
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	22/10/24
\brief	This files implements the logic system that handles scripting of each entity within the game

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "PreCompile.h"
#include "Logic.h"
#include "../ECS/ECS.h"

namespace Ukemochi
{
    LogicSystem::LogicSystem()
    {
        ScriptingEngine::GetInstance().Init();
    }

    void LogicSystem::Init() const
    {
        for (auto& entity : m_Entities)
        {
            auto& logic = ECS::GetInstance().GetComponent<Script>(entity);

            ScriptingEngine::InvokeMethod(ScriptingEngine::GetObjectFromGCHandle(logic.handle), "Start");
        }
    }

    void LogicSystem::Update() const
    {
        for (auto& entity : m_Entities)
        {
            auto& logic = ECS::GetInstance().GetComponent<Script>(entity);

            ScriptingEngine::InvokeMethod(ScriptingEngine::GetObjectFromGCHandle(logic.handle), "Update");
        }
    }

    void LogicSystem::End()
    {
        // auto objects = GameObjectManager::GetInstance().GetAllGOs();
        // for(auto& go : objects)
        // {
        //     go->ReleaseHandle();
        // }
        // 0x4B45414E - Mono's GC will handle the destruction of the script instance when the domain is unloaded or when the application is closed
        // for(auto& entity : m_Entities)
        // {
        // 	auto& scriptObj = ECS::GetInstance().GetComponent<Script>(entity);
        // 	
        // }
    }
}
