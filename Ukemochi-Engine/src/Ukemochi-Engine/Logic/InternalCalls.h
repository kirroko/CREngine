/* Start Header
*****************************************************************/
/*!
\file	InternalCalls.h
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	1/11/24
\brief	This file is forward declaring the internal calls from C# to C++

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include <mono/metadata/object.h> // for MonoObject

#include "Ukemochi-Engine/Factory/GameObjectManager.h"
#include "Ukemochi-Engine/Input/Input.h"

namespace Ukemochi
{
    namespace InternalCalls
    {
        /**
         * \brief Retrieves a managed MonoObject instance of a GameObject by its ID.
         * \param id The unique identifier of the GameObject.
         * \param objectName The name of the GameObject.
         * \return A MonoObject pointer representing the managed instance of the GameObject.
         */
        EXTERN_C UME_API inline MonoObject* GetMonoObject(const uint64_t id, MonoString* objectName)
        {
            const auto& go = GameObjectManager::GetInstance().GetGO(id);
            return go.GetManagedInstance(mono_string_to_utf8(objectName));
        }

        /**
         * \brief Adds a component to a GameObject by its ID and component type name.
         * \param id The unique identifier of the GameObject.
         * \param typeName The name of the component type to add.
         */
        EXTERN_C UME_API inline void AddComponent(uint64_t id, MonoObject* component, MonoString* typeName)
        {
            auto& go = GameObjectManager::GetInstance().GetGO(id);

            std::string typeNameStr = mono_string_to_utf8(typeName);
            auto it = GameObjectManager::componentRegistry.find(typeNameStr);
            if (it != GameObjectManager::componentRegistry.end())
            {
                it->second(go, component);
            }
            else
            {
                UME_ENGINE_WARN("Component type '{0}' not found in registry", typeNameStr); // check registry GOmanager
            }
        }

        EXTERN_C UME_API inline bool GetKey(int32_t key)
        {
            return Input::IsKeyPressed(key);
        }

        EXTERN_C UME_API inline bool GetKeyDown(int32_t key)
        {
            return Input::IsKeyTriggered(key);
        }

        EXTERN_C UME_API inline void LogMessage(MonoString* message)
        {
            UME_ENGINE_INFO("Ukemochi log: {0}", std::string(mono_string_to_utf8(message)));
        }
    }
}
