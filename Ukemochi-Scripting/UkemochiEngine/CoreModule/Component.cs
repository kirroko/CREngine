/* Start Header
 *****************************************************************/
/*!
\file	Component.cs
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	29/10/24
\brief	This files is responsible for the base class for all components in the engine

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
 *******************************************************************/

using System;
using System.Reflection;
using System.Runtime.InteropServices;

namespace UkemochiEngine.CoreModule
{
    public abstract class Component : Object
    {
        // ==================== PROPERTIES ====================
        public GameObject gameObject { get; private set; }
        
        // ==================== METHODS =======================
        public void AddComponent<T>() where T : Component
        {
            // gameObject.AddComponent<typeof(T).BaseType>();
        }

        public T GetComponent<T>() where T : Component
        {
            EngineInterop.LogMessage("Getting component");
            return gameObject.GetComponent<T>();
        }
        
        // ==================== INTERNAL METHODS =======================
        internal void SetGameObject(GameObject obj)
        {
            gameObject = obj;
        }
    }
}