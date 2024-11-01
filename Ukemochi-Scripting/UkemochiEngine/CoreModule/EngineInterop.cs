/* Start Header
 *****************************************************************/
/*!
\file	EngineInterop.cs
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	29/10/24
\brief	This files is responsible for handling the interop between C# and C++ code, specifically for the engine

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
 *******************************************************************/

using System;
using System.Runtime.CompilerServices;

namespace UkemochiEngine.CoreModule
{
    public static class EngineInterop
    {
        // [MethodImplAttribute(MethodImplOptions.InternalCall)]
        // public static extern void AddComponent(ulong id, string typeName);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void AddComponent(ulong id, object component, string typeName);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern object GetObject(ulong id);
        // [MethodImplAttribute(MethodImplOptions.InternalCall)] Example of how to call a C++ function from C#
        // public static extern void LogMessage(string message);
    }
}