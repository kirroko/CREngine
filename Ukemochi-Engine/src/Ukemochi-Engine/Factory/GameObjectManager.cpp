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

using namespace Ukemochi;

GameObject& GameObjectManager::CreateObject()
{
    auto go = std::make_unique<GameObject>(GameObjectFactory::CreateObject());
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

