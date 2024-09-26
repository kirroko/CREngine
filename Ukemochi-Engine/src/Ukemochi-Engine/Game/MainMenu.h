/* Start Header ************************************************************************/
/*!
\file    MainMenu.h
\author  Tan Si Han, t.sihan, 2301264
\par     t.sihan@digipen.edu
\date    20/09/2024
\brief   This files contains the declaration of functions in MainMenu.


Copyright (C) 2024 DigiPen Institute of Technology.  Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#pragma once

void MainMenu_Load();//Load all necessary assets before start of MainMenu

void MainMenu_Initialize();//Initialize the game at the start of MainMenu

void MainMenu_Update();//MainMenu game runtime

void MainMenu_Draw();//rendering of the game for MainMenu

void MainMenu_Free();//release unused assets/variable memories

void MainMenu_Unload();//unload the level