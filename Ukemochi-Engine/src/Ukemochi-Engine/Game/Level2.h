#pragma once
/* Start Header ************************************************************************/
/*!
\file    Level2.h
\author  Tan Si Han, t.sihan, 2301264
\par     t.sihan@digipen.edu
\date    20/09/2024
\brief   This files contains the declaration of functions in Level2.


Copyright (C) 2024 DigiPen Institute of Technology.  Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#pragma once

namespace Ukemochi
{
	void Level2_Load();//Load all necessary assets before start of Level2

	void Level2_Initialize();//Initialize the game at the start of Level2

	void Level2_Update();//Level2 game runtime

	void Level2_Draw();//rendering of the game for Level2

	void Level2_Free();//release unused assets/variable memories

	void Level2_Unload();//unload the level
}