/* Start Header
 *****************************************************************/
/*!
\file	Animation.cs
\author WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date	Nov 17, 2024
\brief	This files is responsible for the animation component of the engine

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
 *******************************************************************/

using System.Runtime.CompilerServices;

namespace Ukemochi
{
    public class Animation : Component
    {
        public bool Play(string clipName)
        {
            return EngineInterop.PlayAnimation(GetInstanceID(), clipName);
        }
    }
}