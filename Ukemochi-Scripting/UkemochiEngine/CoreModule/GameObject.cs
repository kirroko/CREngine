/* Start Header
*****************************************************************/
/*!
\file	GameObject.cs
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	19/10/24
\brief	This files implements basic classes require for engine to function

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
using System;


namespace UkemochiEngine
{
    namespace CoreModule
    { 
        public class GameObject : Object
        {
            public Transform transform { get; }

            public int layer { get; set; }

            public bool activeSelf { get; }

            public bool activeInHierarchy { get; }

            public GameObject(string name) { this.name = name; }

            //public T AddComponent<T>() where T : Component { }

            //public T GetComponent<T>() { }
        }
    }

}
