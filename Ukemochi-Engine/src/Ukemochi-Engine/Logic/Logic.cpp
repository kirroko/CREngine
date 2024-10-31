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
	ScriptingEngine LogicSystem::m_MonoManager; // What the hack
	
	LogicSystem::LogicSystem()
	{
		m_MonoManager.Init();
	}

	void LogicSystem::Init()
	{
		for(auto& entity : m_Entities)
		{
			auto& scriptObj = ECS::GetInstance().GetComponent<Script>(entity);

			if(!scriptObj.instance) // if there's no instance, we gather one
			{
				std::filesystem::path filePath(scriptObj.scriptPath);
				std::string fileName = filePath.stem().string();
				scriptObj.instance = m_MonoManager.InstantiateClass(scriptObj.scriptName, "");

				if(scriptObj.instance)
				{
					UME_ENGINE_ASSERT(false,"No Instance created! Check filename {1}", fileName)
				}
			}

			m_MonoManager.InvokeMethod(static_cast<MonoObject*>(scriptObj.instance), "Start");				 // called on frame 1 before update
			
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
		
			m_MonoManager.InvokeMethod(static_cast<MonoObject*>(logic.instance), "Update");
		}
	}

	void LogicSystem::End()
	{
		// 0x4B45414E - Mono's GC will handle the destruction of the script instance when the domain is unloaded or when the application is closed
		// for(auto& entity : m_Entities)
		// {
		// 	auto& scriptObj = ECS::GetInstance().GetComponent<Script>(entity);
		// 	
		// }
	}
}