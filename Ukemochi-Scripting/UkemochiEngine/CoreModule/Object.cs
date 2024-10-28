/* Start Header
*****************************************************************/
/*!
\file	Object.cs
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	19/10/24
\brief	This files implements basic classes require for engine to function, specifically Object class, the base class for all objects Ukemochi can reference

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
using System;
using System.Runtime.InteropServices;

namespace UkemochiEngine
{
    namespace CoreModule
    {
        public class Object
        {
            [DllImport("__Internal")]
            private static extern ulong GetID();

            /// Members
            public string name { get; set; }

            /// Methods
            /// <summary>
            /// Gets the instance ID of the object
            /// </summary>
            /// <returns>Returns the instance ID of the object</returns>
            public ulong GetInstanceID()
            {
                return GetID();
            }

            // Operators
            public static bool operator ==(Object left, Object right) { return left.Equals(right); }
            public static bool operator !=(Object left, Object right) { return !left.Equals(right); }

            /// <summary>
            /// Removes a GameObject, component or asset
            /// </summary>
            /// <param name="obj">The object to destroy</param>
            /// <param name="t">The optional amount of time to delay before destroying the object</param>
            public static void Destroy(Object obj, float t = 0.0f)
            {

            }

            public override bool Equals(object obj)
            {
                return base.Equals(obj);
            }

            public override int GetHashCode()
            {
                return base.GetHashCode();
            }
        }
    }
}
// 0x4B45414E