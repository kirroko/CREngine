/* Start Header
 *****************************************************************/
/*!
\file	Transform.cs
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	31/10/24
\brief	This files is responsible for the renders of 2D Graphics

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
 *******************************************************************/
namespace UkemochiEngine.CoreModule
{
    public class SpriteRender : Component
    {
        // ==================== FIELDS ====================
        private bool _flipX = false;
        private bool _flipY = false;

        // ==================== PROPERTIES ====================
        public bool FlipX
        {
            get => _flipX;
            set
            {
                _flipX = value;
                EngineInterop.SetSpriteRenderFlipX(GetInstanceID(), _flipX);
            }
        }
        
        public bool FlipY
        {
            get => _flipY;
            set
            {
                _flipY = value;
                EngineInterop.SetSpriteRenderFlipY(GetInstanceID(), _flipY);
            }
        }

        // ==================== METHODS =======================
    }
}