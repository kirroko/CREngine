/* Start Header
*****************************************************************/
/*!
\file	InternalCalls.cpp
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	28/10/24
\brief	This file is responsible for handling internal calls from C# to C++

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "PreCompile.h"
#include "InternalCalls.h"
#include "../Factory/GameObjectManager.h"


using namespace Ukemochi;

namespace InternalCalls
{
    /**
     * \brief Retrieves a managed MonoObject instance of a GameObject by its ID.
     * \param id The unique identifier of the GameObject.
     * \param objectName The name of the GameObject.
     * \return A MonoObject pointer representing the managed instance of the GameObject.
     */
    EXTERN_C UME_API MonoObject* GetObject(const uint64_t id, MonoString* objectName)
    {
        const auto& go = GameObjectManager::GetInstance().GetGO(id);
        return go.GetManagedInstance(mono_string_to_utf8(objectName));
    }

    /**
     * \brief Adds a component to a GameObject by its ID and component type name.
     * \param id The unique identifier of the GameObject.
     * \param typeName The name of the component type to add.
     */
    EXTERN_C UME_API void AddComponent(uint64_t id, MonoObject* component, MonoString* typeName)
    {
        auto& go = GameObjectManager::GetInstance().GetGO(id);

        std::string typeNameStr = mono_string_to_utf8(typeName);
        auto it = GameObjectManager::componentRegistry.find(typeNameStr);
        if(it != GameObjectManager::componentRegistry.end())
        {
            it->second(go, component);
        }
        else
        {
            UME_ENGINE_WARN("Component type '{0}' not found in registry", typeNameStr); // check registry GOmanager
        }
    }
    // EXTERN_C UME_API void AddComponent(uint64_t id, MonoString* typeName)
    // {
    //     UME_ENGINE_TRACE("Ukemochi log: {0}", std::string(mono_string_to_utf8(typeName)));
    //     auto& go = GameObjectManager::GetInstance().GetGO(id);
    //     std::string typeNameStr = mono_string_to_utf8(typeName);
    //     auto it = GameObjectManager::componentRegistry.find(typeNameStr);
    //     if (it != GameObjectManager::componentRegistry.end())
    //     {
    //         it->second(go);
    //     }
    //     else
    //     {
    //         UME_ENGINE_WARN("Component type '{0}' not found in registry", typeNameStr); // check registry GOmanager
    //     }
    // }

    EXTERN_C UME_API void LogMessage(MonoString* message)
    {
        UME_ENGINE_INFO("Ukemochi log: {0}", std::string(mono_string_to_utf8(message)));
    }
}
