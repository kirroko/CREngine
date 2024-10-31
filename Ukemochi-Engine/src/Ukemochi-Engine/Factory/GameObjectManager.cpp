/* Start Header
*****************************************************************/
/*!
\file		GameObjectManager.cpp
\par		Ukemochi
\author     WONG JUN YU, Kean, junyukean.wong, 2301234
\par	    junyukean.wong\@digipen.edu
\par	    Course: CSD2400/CSD2401
\date		29/09/24
\brief		This file is responsible for tracking all game objects in the game

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "PreCompile.h"
#include "GameObjectManager.h"
#include "Factory.h"
#include "Ukemochi-Engine/Logic/Logic.h"

using namespace Ukemochi;

std::unordered_map<std::string,std::function<void(GameObject&)>> GameObjectManager::componentRegistry;

GameObjectManager::GameObjectManager()
{
    RegisterComponents();
}

void GameObjectManager::RegisterComponents()
{// TODO: This is a hacky way to register components for C#, might have to change this.. 3am things
    componentRegistry["Transform"] = [](GameObject& go)
    {
        go.AddComponent(Transform{Vec2(),0.0f,Vec2(1.0f,1.0f)});
        go.SetManagedInstance(LogicSystem::GetMonoManager().InstantiateClass("Transform"));
    };
}

GameObject& GameObjectManager::CreateObject()
{
    auto go = std::make_unique<GameObject>(GameObjectFactory::CreateObject());
    auto id = go->GetInstanceID();
    go->SetManagedInstance(LogicSystem::GetMonoManager().InstantiateClass("GameObject")); // Logic system needs to be constructed first
    void* params[] = { &id }; // TODO: This is a hacky way to pass the entity ID to the script, Might have to check if the data is passed in...
    LogicSystem::GetMonoManager().InvokeMethod(go->GetManagedInstance(), "InvokeSetID", params,1); // Set the entity ID so that scripting knows who to add components too
    go->AddComponent(Transform{Vec2(),0.0f,Vec2(1.0f,1.0f)});
    m_GOs.emplace(id, std::move(go));
    return *m_GOs[id];
}

void GameObjectManager::DestroyObject(EntityID id)
{
    // Before we erase, we tell ECS to destroy the entity
    GameObjectFactory::DestroyObject(*m_GOs[id]);
    m_GOs.erase(id);
}

GameObject& GameObjectManager::GetGO(EntityID id)
{
    return *m_GOs[id];
}

