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

namespace UkemochiEngine.CoreModule
{
    public class Transform : Component
    {
        // ==================== FIELDS ====================
        private Vector2 _position;
        private Vector2 _rotation;
        private Vector2 _scale = new Vector2(1,1);
        
        // ==================== PROPERTIES ====================
        public Vector2 Position
        {
            get => _position;
            set
            {
                _position = value; 
                EngineInterop.SetTransformPosition(GetInstanceID(), _position.x, _position.y);
            }
        }
        public Vector2 Rotation
        {
            get => _rotation;
            set
            {
                _rotation = value;
                EngineInterop.SetTransformRotation(GetInstanceID(), _rotation.x, _rotation.y);
            }
        } // Unity is Quaternion, but for simplicity, we will use Vector2

        public Vector2 Scale
        {
            get => _scale;
            
            set
            {
                _scale = value;
                EngineInterop.SetTransformScale(GetInstanceID(), _scale.x, _scale.y);
            }
        }

        // ==================== METHODS =======================
        
    }
}