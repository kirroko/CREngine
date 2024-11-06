/* Start Header
 *****************************************************************/
/*!
\file	Vector2.cs
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	31/09/24
\brief	This files is responsible for the Vector2 struct that stores 2D coordinates
        for the engine

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
 *******************************************************************/
namespace UkemochiEngine.CoreModule
{
    public struct Vector2
    {
        // ==================== FIELDS ====================
        public float x;
        public float y;
        
        // ==================== METHODS ====================
        public override string ToString()
        {
            return $"({x}, {y})";
        }

        public Vector2(float value)
        {
            x = value;
            y = value;
        }
        public Vector2(float x, float y)
        {
            this.x = x;
            this.y = y;
        }
    }
}