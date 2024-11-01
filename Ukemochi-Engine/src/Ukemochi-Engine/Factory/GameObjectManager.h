/* Start Header
*****************************************************************/
/*!
\file		GameObjectManager.h
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
#pragma once
#include "PreCompile.h"
#include "GameObject.h"

namespace Ukemochi
{
    class GameObjectManager
    {
        GameObjectManager();
        std::unordered_map<EntityID,std::unique_ptr<GameObject>> m_GOs;
    public:
        static GameObjectManager& GetInstance()
        {
            static GameObjectManager instance;
            return instance;
        }
        // Type Registry
        static std::unordered_map<std::string,std::function<void(GameObject&,MonoObject*)>> componentRegistry;
        
        void RegisterComponents();

        /**
         * @brief Create a new game object (that is empty)
         * @return reference to the new created game object
         */
        GameObject& CreateObject(const std::string& name = "GameObject", const std::string& tag = "Default");

        /**
         * @brief Create a new gameobject from a prefab
         * @param prefabPath The path to the prefab
         * @return reference to the new created game object
         */
        GameObject& CreateObject(const std::string& prefabPath);

        void DestroyObject(EntityID id);

        GameObject& GetGO(EntityID id);
    };
}
