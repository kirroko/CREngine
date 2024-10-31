/* Start Header
 *****************************************************************/
/*!
\file	Transform.cs
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	31/10/24
\brief	This files is

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
 *******************************************************************/

using System.Collections.Generic;
using System.Linq;

// For the love of all things, here's me writing down my thoughts
// C++ will InstantiateClass of this class as an Object when a user of the editor calls add gameobject
// This object holds a list of components, and the first component is always the transform
// Follow the same principle as above, when a user call add component from the editor, the engine will instantiate the class of that component

// 0342 1/11/24 - Mr Wong, Please sleep, We last stop off completing the C++ C# 1:1 relationship, and now we have to finish the C# library for scripting
namespace UkemochiEngine.CoreModule
{
    public class GameObject : Object
    {
        // ==================== PROPERTIES ====================
        public Transform transform { get; private set; } // Read-only
        
        private List<Component> _components = new List<Component>();
        
        // ==================== METHODS =======================
        public GameObject()
        {
            AddComponent<Transform>();
        }
        
        public T AddComponent<T>() where T : Component, new()
        {
            T component = new T();
            EngineInterop.AddComponent(GetInstanceID(), typeof(T).Name);
            _components.Add(component);
            return component;
        }

        public T GetComponent<T>() where T : Component
        {
            return _components.OfType<T>().FirstOrDefault();
        }
        
        // ==================== INTERNAL INVOKES ====================
        // TODO: Remember to add new components for C++ to instantiate C# side
        internal void AddTransformComponent()
        {
            _components.Add(new Transform());
        }
    }
}