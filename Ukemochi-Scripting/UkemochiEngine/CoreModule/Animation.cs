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

        public bool Play(string clipName, int startFrame, int endFrame)
        {
            return EngineInterop.PlayAnimationWithFrame(GetInstanceID(), clipName, startFrame, endFrame);
        }
        
        public bool PlayQueued(string clipName)
        {
            return EngineInterop.PlayQueuedAnimation(GetInstanceID(), clipName);
        }

        public bool PlayImmediate(string clipName)
        {
            return EngineInterop.PlayImmediately(GetInstanceID(), clipName);
        }
        
        public bool IsPlaying()
        {
            return EngineInterop.IsAnimationPlaying(GetInstanceID());
        }
    }
}