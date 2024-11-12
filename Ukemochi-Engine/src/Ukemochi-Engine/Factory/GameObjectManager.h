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
        std::unordered_map<EntityID, std::unique_ptr<GameObject>> m_GOs;

    public:
        static GameObjectManager& GetInstance()
        {
            static GameObjectManager instance;
            return instance;
        }

        /**
         * @brief Create a new gameobject from a prefab
         * @param prefabPath The path to the prefab
         * @return reference to the new created game object
         */
        GameObject& CreatePrefabObject(const std::string& prefabPath);

        /**
         * @brief Create a new game object (that is empty)
         * @return reference to the new created game object
         */
        GameObject& CreateObject(const std::string& name = "GameObject", const std::string& tag = "Default");

        /**
         * @brief Clone a game object
         * @param targetGO The game object to clone
         * @param  name The name of the new game object
         * @param tag The tag of the new game object
         * @return reference to the new created game object
         */
        GameObject& CloneObject(const GameObject& targetGO, const std::string& name = "GameObject",
                                const std::string& tag = "Default");

        /**
         * @brief Destroy a game object
         * @param id The id of the game object to destroy
         */
        void DestroyObject(EntityID id);

        /**
         * @brief Get a gameobject by its tag
         * @param tag a tag to search for
         * @return a gameobject
         */
        GameObject* GetGOByTag(const std::string& tag) const;

        /**
         * @brief Get a game object by its id
         * @param id The id of the game object
         * @return reference to the game object
         */
        GameObject* GetGO(EntityID id);

        /**
         * @brief Get all game objects
         * @return vector of game objects
         */
        std::vector<GameObject*> GetAllGOs() const;

        /**
         * @brief Initialize all the MonoGCHandles for the game objects
         */
        void InitAllHandles() const;

        /**
         * @brief Release all the MonoGCHandles for the game objects
         */
        void ReleaseAllHandles() const;
    };
}
