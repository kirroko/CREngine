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

// Static member initialization, this is where we register a function to create a component via a name.
// C# will call addcomponent with the component name and the instance of the component
// We will then call the function to create the component and add it to the GameObject in C++ side
std::unordered_map<std::string,std::function<void(GameObject&,MonoObject*)>> GameObjectManager::componentRegistry;

GameObjectManager::GameObjectManager()
{
    RegisterComponents();
}

void GameObjectManager::RegisterComponents()
{
    // TODO: This is a hacky way to register components for C#, might have to change this.. 
    componentRegistry["Transform"] = [](GameObject& go, MonoObject* instance)
    {
        go.SetManagedComponentInstance(instance, "Transform");
    };
    // TODO: More components to be added here
}

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
    GameObjectFactory::DestroyObject(*m_GOs[id]);
    m_GOs.erase(id);
}

GameObject& GameObjectManager::GetGO(EntityID id)
{
    return *m_GOs[id];
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

