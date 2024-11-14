/* Start Header
 *****************************************************************/
/*!
\file	Input.cs
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	29/10/24
\brief	This files is responsible for handling the input from the user

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
 *******************************************************************/
namespace Ukemochi
{
    public static class Input
    {
        public static bool GetKey(KeyCode key)
        {
            return EngineInterop.GetKey((int)key);
        }

        public static bool GetKeyDown(KeyCode key)
        {
            return EngineInterop.GetKeyDown((int)key);
        }
        
        public static bool GetMouseButton(KeyCode button)
        {
            return EngineInterop.GetMouseButton((int)button);
        }

        public static bool GetMouseButtonDown(KeyCode button)
        {
            return EngineInterop.GetMouseButtonDown((int)button);
        }
    }
}