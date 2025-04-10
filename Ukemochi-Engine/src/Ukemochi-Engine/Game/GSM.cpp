/* Start Header ************************************************************************/
/*!
\file       GSM.cpp
\author     Tan Si Han, t.sihan, 2301264, t.sihan@digipen.edu
\date       Sept 20, 2024
\brief      This files contains the definition of the GameStateManager which handles the game state.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "GameStateList.h"
#include "GSM.h"
#include <iostream>

namespace Ukemochi
{
	int gsm_current = 0, gsm_previous = 0, gsm_next = 0;

	int es_current = 0;

	FP gsm_fpLoad = nullptr, gsm_fpInitialize = nullptr, gsm_fpUpdate = nullptr, gsm_fpDraw = nullptr, gsm_fpFree = nullptr, gsm_fpUnload = nullptr;

	/*!***********************************************************************
	\brief
		Initializes the GameStateManager with the starting state.
	\param startingState
		The initial state to set for the game.
	\details
		This function sets up the state manager and prepares the game
		to start in the specified state.
	*************************************************************************/
	void GSM_Initialize(int startingState) // initialize the state manager
	{
		gsm_current = gsm_previous = gsm_next = startingState;

		// some additional code
		// std::cout << "GSM:Initialize" << '\n';
	}

	/*!***********************************************************************
	\brief
		Initializes the GameStateManager with the starting state.
	\param startingState
		The initial state to set for the game.
	\details
		This function sets up the state manager and prepares the game
		to start in the specified state.
	*************************************************************************/
	void GSM_Update() // updating the gamestate when the game is running
	{
		// some unfinished code here
		// std::cout << "GSM:Update" << '\n';

		switch (gsm_current)
		{
		case GS_MAINMENU:
			// change gamestate to level mainmenu
			//gsm_fpLoad = MainMenu_Load;
			//gsm_fpInitialize = MainMenu_Initialize;
			//gsm_fpUpdate = MainMenu_Update;
			//gsm_fpDraw = MainMenu_Draw;
			//gsm_fpFree = MainMenu_Free;
			//gsm_fpUnload = MainMenu_Unload;
			break;
		case GS_LEVEL1:
			// change gamestate to level 1
			//gsm_fpLoad = Level1_Load;
			//gsm_fpInitialize = Level1_Initialize;
			//gsm_fpUpdate = Level1_Update;
			//gsm_fpDraw = Level1_Draw;
			//gsm_fpFree = Level1_Free;
			//gsm_fpUnload = Level1_Unload;
			break;
		case GS_LEVEL2:
			// change gamestate to level 1
			//gsm_fpLoad = Level2_Load;
			//gsm_fpInitialize = Level2_Initialize;
			//gsm_fpUpdate = Level2_Update;
			//gsm_fpDraw = Level2_Draw;
			//gsm_fpFree = Level2_Free;
			//gsm_fpUnload = Level2_Unload;
			break;
		case GS_RESTART:
			break;
		default:
			break;
		}
	}
}