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
#include "Ukemochi-Engine/Factory/GameObjectManager.h"

namespace Ukemochi
{
    LogicSystem::LogicSystem()
    {
        ScriptingEngine::GetInstance().Init();
        // ScriptingEngine LogicSystem::m_MonoManager; // What the hack
    }

    void LogicSystem::Init()
    {
        for (auto& entity : m_Entities)
        {
            auto& scriptObj = ECS::GetInstance().GetComponent<Script>(entity);

            if (!scriptObj.instance) // if there's no instance, we gather one
            {
                std::filesystem::path filePath(scriptObj.scriptPath);
                std::string fileName = filePath.stem().string();
                // We instantiate the client's script here
                // TODO: DID WE FORGET TO INSTANTIATE THE CLIENT'S ASSEMBLY?? 1/11/24 3:16pm
                scriptObj.instance = ScriptingEngine::GetInstance().InstantiateClientClass(scriptObj.scriptName);
                if (scriptObj.instance)
                {
                    UME_ENGINE_ASSERT(false, "No Instance created! Check filename {1}", fileName)
                }
            }
            // called on frame 1 before update
            ScriptingEngine::GetInstance().InvokeMethod(static_cast<MonoObject*>(scriptObj.instance), "Start");
        }
    }

    void LogicSystem::Update()
    {
        // For my overwhelm brain to understand
        // logic system updates all the C# scripts with their respective data (Transform, Rigidbody2D, etc.)
        // and then we will call a function to retrieve the data from the C# side and update the Entity's component
        for (auto& entity : m_Entities)
        {
            auto& logic = ECS::GetInstance().GetComponent<Script>(entity);

            ScriptingEngine::GetInstance().InvokeMethod(static_cast<MonoObject*>(logic.instance), "Update");
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
