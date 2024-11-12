/* Start Header
 *****************************************************************/
/*!
\file	Debug.cs
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	04/11/24
\brief	This files is responsible for logging message to C++ side for debugging purposes

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
 *******************************************************************/
namespace Ukemochi
{
    public static class Debug
    {
        public static void Log(string message)
        {
            EngineInterop.LogMessage(message);
        }
        
        public static void LogWarning(string message)
        {
            EngineInterop.LogWarning(message);
        }
    }
}