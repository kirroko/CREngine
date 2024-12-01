/* Start Header ************************************************************************/
/*!
\file       GSM.h
\author     Tan Si Han, t.sihan, 2301264, t.sihan@digipen.edu
\date       Sept 20, 2024
\brief      This files contains the declaration of the GameStateManager which handles the game state.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include "GameStateList.h"
namespace Ukemochi
{
    /*!***********************************************************************
	\typedef FP
	\brief
       Defines a function pointer type for managing game state callbacks.
	 \details
       This typedef is used for functions that manage various operations
       such as loading, initializing, updating, drawing, freeing, and unloading
       game states.
	*************************************************************************/
	typedef void(*FP)(void);

	// Global variables for managing game states
	extern int gsm_current, gsm_previous, gsm_next;

	extern int es_current;

	extern FP gsm_fpLoad, gsm_fpInitialize, gsm_fpUpdate, gsm_fpDraw, gsm_fpFree, gsm_fpUnload;

    /*!***********************************************************************
    \brief
        Initializes the GameStateManager with the starting state.
    \param startingState
        The initial state to set for the game.
    \details
        This function sets up the state manager and prepares the game
        to start in the specified state.
    *************************************************************************/
	void GSM_Initialize(int startingState);//initialize the state manager

    /*!***********************************************************************
    \brief
        Initializes the GameStateManager with the starting state.
    \param startingState
        The initial state to set for the game.
    \details
        This function sets up the state manager and prepares the game
        to start in the specified state.
    *************************************************************************/
    void GSM_Update();//updating the gamestate when the game is running
}