/* Start Header
 *****************************************************************/
/*!
\file	Transform.cs
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	31/10/24
\brief	This files is responsible for the transform component of the engine

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
 *******************************************************************/

using System.Globalization;

namespace Ukemochi
{
    public class Transform : Component
    {
        // ==================== PROPERTIES ====================
        public Vector2 position
        {
            get
            {
                float x = 0.0f;
                float y = 0.0f;
                EngineInterop.GetTransformPosition(GetInstanceID(), ref x, ref y);
                return new Vector2(x, y);
            }
            set => EngineInterop.SetTransformPosition(GetInstanceID(), value.x, value.y);
        }

        public Vector2 rotation
        {
            get
            {
                float x = 0.0f;
                float y = 0.0f;
                EngineInterop.GetTransformRotation(GetInstanceID(), ref x, ref y);
                return new Vector2(x, y);
            }
            set => EngineInterop.SetTransformRotation(GetInstanceID(), value.x, value.y);
        } // Unity is Quaternion, but for simplicity, we will use Vector2

        public Vector2 scale
        {
            get
            {
                float x = 0.0f;
                float y = 0.0f;
                EngineInterop.GetTransformScale(GetInstanceID(), ref x, ref y);
                return new Vector2(x, y);
            }
            set => EngineInterop.SetTransformScale(GetInstanceID(), value.x, value.y);
        }
        // ==================== METHODS =======================
    }
}