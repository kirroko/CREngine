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
namespace UkemochiEngine.CoreModule
{
    public class Transform : Component
    {
        // ==================== PROPERTIES ====================
        public Vector2 Position { get; set; } = new Vector2();
        public Vector2 Rotation { get; set; } = new Vector2(); // Unity is Quaternion, but for simplicity, we will use Vector2
        public Vector2 Scale { get; set; } = new Vector2(1, 1);

        // ==================== METHODS =======================
        
    }
}