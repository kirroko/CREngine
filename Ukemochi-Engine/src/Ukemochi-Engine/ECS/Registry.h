/* Start Header
*****************************************************************/
/*!
\file Registry.h
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par junyukean.wong\@digipen.edu
\date 15/09/24
\brief A registry where all components are stored.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once

#include <unordered_map>
#include "Entity.h"

namespace Ukemochi
{
    class IComponentArray
    {
    public:
        virtual ~IComponentArray() = default;
        virtual void EntityDestroyed(Entity entity) = 0;
    };

    // The component array class that stores the components of type T in a contiguous array.
    template <typename T>
    class ComponentArray : public IComponentArray
    {
    public:
        void InsertData(Entity entity, T component);

        void RemoveData(Entity entity);

        T &GetData(Entity entity);

        void EntityDestroyed(Entity entity) override;

    private:
        // The packed array of components (of generic type T),
        // set to a specified maximum amount, matching the maximum number
        // of entities allowed to exist simultaneously, so that each entity
        // has a unique spot.
        std::array<T, MAX_ENTITIES> m_ComponentArray;

        // Map from an entity ID to an array index.
        std::unordered_map<Entity, size_t> m_EntityToIndexMap;

        // Map from an array index to an entity ID.
        std::unordered_map<size_t, Entity> m_IndexToEntityMap;

        // Total size of valid entries in the array.
        size_t m_Size;
    };
}