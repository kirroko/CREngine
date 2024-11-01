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

namespace Ukemochi
{
    namespace InternalCalls
    {
        EXTERN_C MonoObject* GetObject(uint64_t id, MonoString* objectName);
        EXTERN_C UME_API void AddComponent(uint64_t id, MonoObject* component, MonoString* typeName);
        EXTERN_C void LogMessage(MonoString* message);
    }
}