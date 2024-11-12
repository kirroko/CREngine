/* Start Header
 *****************************************************************/
/*!
\file	Transform.cs
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	31/10/24
\brief	This files is responsible for handling all the gameobject's components, and the first component is always the transform

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
 *******************************************************************/

using System;

// For the love of all things, here's me writing down my thoughts
// C++ will InstantiateClass of this class as an Object when a user of the editor calls add gameobject
// This object holds a list of components, and the first component is always the transform
// Follow the same principle as above, when a user call add component from the editor, the engine will instantiate the class of that component

namespace Ukemochi
{
    public class GameObject : Object
    {
        // ==================== PROPERTIES ====================
        public Transform transform { get; private set; } // Read-only
        
        // ==================== METHODS =======================
        public bool HasComponent<T>() where T : Component
        {
            Type componentType = typeof(T);
            return EngineInterop.HasComponent(GetInstanceID(), componentType);
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;
            if (transform == null)
            {
                transform = GetComponent<Transform>();
                transform.SetGameObject(this);
                _id = GetInstanceID();
            }

            T component = new T();
            component._id = GetInstanceID();
            component.SetGameObject(this);
            return component;
        }
        
        public static GameObject FindWithTag(string tag)
        {
            EngineInterop.GetObjectByTag(tag, out ulong id);
            if (id == 999999)
                return null;
            
            var obj = new GameObject();
            obj._id = id;
            obj.transform = new Transform();
            obj.transform.SetGameObject(obj);
            obj.transform._id = id;
            return obj;
        }

        public override string ToString()
        {
            return base.ToString();
        }
    }
}