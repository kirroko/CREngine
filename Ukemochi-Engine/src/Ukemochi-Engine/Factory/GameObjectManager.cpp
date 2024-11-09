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

GameObject& GameObjectManager::CreateObject(const std::string& name, const std::string& tag)
{
    auto go = std::make_unique<GameObject>(GameObjectFactory::CreateObject(name, tag));
    auto id = go->GetInstanceID();
    m_GOs.emplace(id, std::move(go));
    return *m_GOs[id];
}

GameObject& GameObjectManager::CloneObject(const GameObject& targetGO, const std::string& name, const std::string& tag)
{
    auto go = std::make_unique<GameObject>(GameObjectFactory::CloneObject(targetGO,name,tag));
    auto id = go->GetInstanceID();
    m_GOs.emplace(id, std::move(go));
    return *m_GOs[id];
}

GameObject& GameObjectManager::CreatePrefabObject(const std::string& prefabPath)
{
    auto go = std::make_unique<GameObject>(GameObjectFactory::CreatePrefebObject(prefabPath));
    auto id = go->GetInstanceID();
    m_GOs.emplace(id, std::move(go));
    return *m_GOs[id];
}

void GameObjectManager::DestroyObject(EntityID id)
{
    // Before we erase, we tell ECS to destroy the entity
    if(ECS::GetInstance().HasComponent<Script>(id))
    {
        ECS::GetInstance().GetComponent<Script>(id).instance = nullptr;
        ScriptingEngine::DestroyGCHandle(ECS::GetInstance().GetComponent<Script>(id).handle);
    }
    GameObjectFactory::DestroyObject(*m_GOs[id]);
    m_GOs.erase(id);
}

GameObject* GameObjectManager::GetGOByTag(const std::string& tag) const
{
    for(auto& go : m_GOs)
    {
        if(go.second->GetTag() != tag)
            continue;

        return go.second.get();
    }
    
    return nullptr;
}

GameObject* GameObjectManager::GetGO(EntityID id)
{
    auto it = m_GOs.find(id);
    if (it != m_GOs.end())
    {
        return it->second.get();
    }
    UME_ENGINE_ERROR("GameObject with ID {0} not found", id);
    return nullptr;
}

std::vector<GameObject*> GameObjectManager::GetAllGOs() const
{
    std::vector<GameObject*> gos;
    for (auto& go : m_GOs)
    {
        gos.push_back(go.second.get());
    }
    return gos;
}

void GameObjectManager::InitAllHandles() const
{
    for(auto& go : m_GOs)
    {
        if(ECS::GetInstance().HasComponent<Script>(go.first))
        {
            auto& script = ECS::GetInstance().GetComponent<Script>(go.first);
            script.instance = ScriptingEngine::GetInstance().InstantiateClientClass(script.scriptName);
            EntityID id = go.first;
            ScriptingEngine::SetMonoFieldValueULL(static_cast<MonoObject*>(script.instance), "_id", &id);
            script.handle = ScriptingEngine::CreateGCHandle(static_cast<MonoObject*>(script.instance));
        }
    }
}

void GameObjectManager::ReleaseAllHandles() const
{
    for(auto& go : m_GOs)
    {
        if(ECS::GetInstance().HasComponent<Script>(go.first))
        {
            ECS::GetInstance().GetComponent<Script>(go.first).instance = nullptr;
            ScriptingEngine::DestroyGCHandle(ECS::GetInstance().GetComponent<Script>(go.first).handle);
        }
    }
}
