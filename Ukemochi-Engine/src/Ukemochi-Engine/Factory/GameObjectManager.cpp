/* Start Header ************************************************************************/
/*!
\file       GameObjectManager.cpp
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date       Sep 29, 2024
\brief      This file is responsible for tracking all game objects in the game.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "GameObjectManager.h"
#include "Factory.h"
#include "Ukemochi-Engine/Logic/Logic.h"

using namespace Ukemochi;

/**
 * @brief Create a new GameObject
 *
 * @return GameObject&  newly created GameObject
 */
GameObject& GameObjectManager::CreateObject(const std::string& name, const std::string& tag)
{
    auto go = std::make_unique<GameObject>(GameObjectFactory::CreateObject(name, tag));
    auto id = go->GetInstanceID();
    m_GOs.emplace(id, std::move(go));
    return *m_GOs[id];
}

/**
 * @brief Clone a GameObject
 *
 * @param targetGO the GameObject to clone
 * @param name
 * @param tag
 * @return GameObject& a reference to the cloned GameObject
 */
GameObject& GameObjectManager::CloneObject(const GameObject& targetGO, const std::string& name, const std::string& tag)
{
    auto go = std::make_unique<GameObject>(GameObjectFactory::CloneObject(targetGO,name,tag));
    auto id = go->GetInstanceID();
    m_GOs.emplace(id, std::move(go));
    return *m_GOs[id];
}

/**
 * @brief Create a new GameObject with components from a JSON file
 *
 * @param prefabPath the path to the JSON file
 * @return GameObject& a reference to the newly created GameObject
 */
GameObject& GameObjectManager::CreatePrefabObject(const std::string& prefabPath)
{
    auto go = std::make_unique<GameObject>(GameObjectFactory::CreatePrefebObject(prefabPath));
    auto id = go->GetInstanceID();
    m_GOs.emplace(id, std::move(go));
    return *m_GOs[id];
}

/**
 * @brief Destroy an existing game object
 *
 * @param id The entity ID of the game object to destroy
 */
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

/**
 * @brief Get a GameObject by its tag
 * @param tag The tag of the GameObject
 * @return GameObject* The GameObject with the tag
 */
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

/**
 * @brief Get a GameObject by its ID
 * @param id The ID of the GameObject
 * @return GameObject* The GameObject with the ID
 */
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

/**
 * @brief Get all GameObjects
 * @return std::vector<GameObject*> A vector of all GameObjects
 */
std::vector<GameObject*> GameObjectManager::GetAllGOs() const
{
    std::vector<GameObject*> gos;
    for (auto& go : m_GOs)
    {
        gos.push_back(go.second.get());
    }
    return gos;
}

/**
 * @brief Helper function to initialize all gameobject's script handles
 */
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

/**
 * @brief Helper function to release all gameobject's script handles
 */
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

/**
 * @brief Create an empty GameObject
 * @param name The name of the GameObject
 * @param tag The tag of the GameObject
 * @return GameObject& The newly created GameObject
 */
GameObject& GameObjectManager::CreateEmptyObject(const std::string& name, const std::string& tag)
{
    // Use the existing GameObjectFactory to create a new object
    auto go = std::make_unique<GameObject>(GameObjectFactory::CreateObject(name, tag));
    auto id = go->GetInstanceID();
    m_GOs.emplace(id, std::move(go));
    return *m_GOs[id]; // Return reference to the newly created object
}
