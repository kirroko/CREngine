/* Start Header
 *****************************************************************/
/*!
\file	Time.cs
\author WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date	Nov 18, 2024
\brief	This files is provides an interface to get time information from ukemochi engine

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
 *******************************************************************/
namespace Ukemochi
{
    public static class Time
    {
        public static float deltaTime => EngineInterop.GetDeltaTime();

        public static float fixedDeltaTime => EngineInterop.GetFixedDeltaTime();
    }
}