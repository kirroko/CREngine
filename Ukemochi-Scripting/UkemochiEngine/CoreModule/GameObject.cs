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

using System.Collections.Generic;
using System.Linq;

// For the love of all things, here's me writing down my thoughts
// C++ will InstantiateClass of this class as an Object when a user of the editor calls add gameobject
// This object holds a list of components, and the first component is always the transform
// Follow the same principle as above, when a user call add component from the editor, the engine will instantiate the class of that component

namespace UkemochiEngine.CoreModule
{
    public class GameObject : Object
    {
        // ==================== PROPERTIES ====================
        public Transform transform { get; private set; } // Read-only
        
        private Dictionary<string,Component> _dictionaryComponents = new Dictionary<string, Component>();
        
        // ==================== METHODS =======================
        public GameObject()
        {
            // var compo = AddComponent<Transform>();
            // compo.SetGameOjbect(this);
        }
        
        public T AddComponent<T>() where T : Component, new()
        {
            T component = new T();
            component.SetGameObject(this);
            _dictionaryComponents.Add(typeof(T).Name, component);
            EngineInterop.AddComponent(GetInstanceID(), component, typeof(T).Name);
            return component;
        }

        public T GetComponent<T>() where T : Component
        {
            EngineInterop.LogMessage("Gameobject getting component");
            var component = _dictionaryComponents[typeof(T).Name];
            return (T)component;
            // return _components.OfType<T>().FirstOrDefault();
        }
        
        // ==================== INTERNAL INVOKES ====================
        // TODO: Remember to add new components for C++ to instantiate C# side
        internal void AddTransformComponent(object objTransform)
        {
            if (!(objTransform is Transform obj))
            {
                EngineInterop.LogMessage("Transform is null");
                return;
            }
            obj.SetGameObject(this);
            
            _dictionaryComponents.Add(nameof(Transform), obj);
            // _components.Add(obj);
        }

        internal void AddRigidbody2DComponent(object rigidbody2D)
        {
            if (!(rigidbody2D is Rigidbody2D obj))
            {
                EngineInterop.LogMessage("Rigidbody2D is null");
                return;
            }
            obj.SetGameObject(this);
            _dictionaryComponents.Add(nameof(Rigidbody2D), obj);
            // _components.Add(obj);
        }

        internal void AddScriptComponent(object script)
        {
            // Script is a bit different, as the class is defined by the client
            // Client's script all derive from BaseScript, so do we instantiate BaseScript?
            if (!(script is BaseScript obj))
            {
                EngineInterop.LogMessage("Script is null");
                return;
            }
            obj.SetGameObject(this);
            _dictionaryComponents.Add(nameof(BaseScript), obj);
            // _components.Add(obj);
        }

        internal void AddSpriteRenderComponent(object spriteRender)
        {
            if (!(spriteRender is SpriteRender obj))
            {
                EngineInterop.LogMessage("SpriteRender is null");
                return;
            }
            obj.SetGameObject(this);
            _dictionaryComponents.Add(nameof(spriteRender), obj);
            // _components.Add(obj);
        }

        internal void AddBoxCollider2DComponent(object boxCollider2D)
        {
            if (!(boxCollider2D is BoxCollider2D obj))
            {
                EngineInterop.LogMessage("BoxCollider2D is null");
                return;
            }
            obj.SetGameObject(this);
            _dictionaryComponents.Add(nameof(BoxCollider2D), obj);
            // _components.Add(obj);
        }
    }
}