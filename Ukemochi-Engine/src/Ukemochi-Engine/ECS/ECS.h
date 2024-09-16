/* Start Header
*****************************************************************/
/*!
\file ECS.h
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par junyukean.wong\@digipen.edu
\date 15/09/24
\brief The ECS will coordinator between the Component, Entity and System managers

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once

#include "Component.h" // ComponentManager
#include "Entity.h"    // EntityManager
#include "Systems.h"   // SystemManager

namespace Ukemochi
{
    class ECS
    {
        std::unique_ptr<ComponentManager> m_ComponentManager;
        std::unique_ptr<EntityManager> m_EntityManager;
        std::unique_ptr<SystemManager> m_SystemManager;

    public:
        // Create pointers to each manager
        void Init();

        Entity CreateEntity();

        void DestroyEntity(Entity entity);
        
        // Component methods
        template<typename T>
        void RegisterComponent();

        template<typename T>
        void AddComponent(Entity entity, T component);

        template<typename T>
        void RemoveComponent(Entity entity);

        template<typename T>
        T& GetComponent(Entity entity);

        template<typename T>
        ComponentType GetComponentType();

        // System methods
        template<typename T>
        std::shared_ptr<T> RegisterSystem();

        template<typename T>
        void SetSystemSignature(Signature signature);
    };
}
