/* Start Header
*****************************************************************/
/*!
\file	Vector2.cs
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	19/10/24
\brief	This files implements the Vector2 structure

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
        public struct Vector2
        {
            // Members
            public float x;
            public float y;

            // ctor
            public Vector2 (float x, float y)
            {
                this.x = x;
                this.y = y;
            }
        }
    }
}
