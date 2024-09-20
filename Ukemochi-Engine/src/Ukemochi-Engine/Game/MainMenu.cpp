/* Start Header ************************************************************************/
/*!
\file    MainMenu.cpp
\author  Tan Si Han, t.sihan, 2301264
\par     t.sihan@digipen.edu
\date    20/09/2024
\brief   This files contains the definction of functions in MainMenu.


Copyright (C) 2024 DigiPen Institute of Technology.  Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#include "PreCompile.h"

#include "GSM.h"
#include "MainMenu.h"
#include <iostream>

int MainMenu_Counter;

void MainMenu_Load()//Load all necessary assets before start of MainMenu
{
	//some code here
	//std::cout << "MainMenu:Load" << '\n';

	MainMenu_Counter = 1;
}

void MainMenu_Initialize()//Initialize the game at the start of MainMenu
{
	//some code here
	//std::cout << "MainMenu:Initialize" << '\n';
}

void MainMenu_Update()//MainMenu game runtime
{
	//some code here
	//std::cout << "MainMenu:Update" << '\n';

	MainMenu_Counter = 1;

	if (MainMenu_Counter == 0)//when counter become 0
	{
		//switch to Level 1
		gsm_next = GS_LEVEL1;
	}
}

void MainMenu_Draw()//rendering of the game for MainMenu
{
	//some code here
	//std::cout << "MainMenu:Draw" << '\n';
}

void MainMenu_Free()//release unused assets/variable memories
{
	//some code here
	//std::cout << "MainMenu:Free" << '\n';
}

void MainMenu_Unload()//unload the level
{
	//some code here
	//std::cout << "MainMenu:Unload" << '\n';
}