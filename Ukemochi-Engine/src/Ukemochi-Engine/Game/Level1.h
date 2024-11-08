/* Start Header ************************************************************************/
/*!
\file    Level1.h
\author  Tan Si Han, t.sihan, 2301264
\par     t.sihan@digipen.edu
\date    20/09/2024
\brief   This files contains the declaration of functions in Level1.


Copyright (C) 2024 DigiPen Institute of Technology.  Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#pragma once

namespace Ukemochi
{
	void Level1_Load();//Load all necessary assets before start of Level1

	void Level1_Initialize();//Initialize the game at the start of Level1

	void Level1_Update();//Level1 game runtime

	void Level1_Draw();//rendering of the game for Level1

	void Level1_Free();//release unused assets/variable memories

	void Level1_Unload();//unload the level

	void print_performance(std::chrono::duration<double> loop, std::chrono::duration<double> collision, std::chrono::duration<double> physics, std::chrono::duration<double> graphics);
}