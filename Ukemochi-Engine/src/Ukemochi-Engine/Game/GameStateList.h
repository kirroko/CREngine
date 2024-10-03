/* Start Header ************************************************************************/
/*!
\file    GameStateLish.h
\author  Tan Si Han, t.sihan, 2301264
\par     t.sihan@digipen.edu
\date    20/09/2024
\brief   This files contains the enum of GameStateList.


Copyright (C) 2024 DigiPen Institute of Technology.  Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#pragma once

namespace Ukemochi
{
	enum GS_STATES
	{
		GS_ENGINE = 0,
		GS_MAINMENU,
		GS_LEVEL1,
		GS_LEVEL2,

		GS_PAUSED,
		GS_QUIT,
		GS_RESTART
	};
}