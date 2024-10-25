/* Start Header ************************************************************************/
/*!
\file    GSM.hpp
\author  Tan Si Han, t.sihan, 2301264
\par     t.sihan@digipen.edu
\date    20/09/2024
\brief   This files contains the declaration of the GameStateManager which handles the game state.


Copyright (C) 2024 DigiPen Institute of Technology.  Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#pragma once
#include "GameStateList.h"
namespace Ukemochi
{

	typedef void(*FP)(void);

	extern int gsm_current, gsm_previous, gsm_next;

	extern int es_current;

	extern FP gsm_fpLoad, gsm_fpInitialize, gsm_fpUpdate, gsm_fpDraw, gsm_fpFree, gsm_fpUnload;

	void GSM_Initialize(int startingState);//initialize the state manager
	void GSM_Update();//updating the gamestate when the game is running
}