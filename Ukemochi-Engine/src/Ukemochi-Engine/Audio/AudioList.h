/* Start Header ************************************************************************/
/*!
\file       AudioList.h
\author     Tan Si Han, t.sihan, 2301264, t.sihan\@digipen.edu
\date       Oct 4, 2024
\brief      This file contains the enum of the Audio system which contain Audio List & Audio Channel.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

namespace Ukemochi
{
	/*!***********************************************************************
	\brief
	 List of all the Audio Files
	*************************************************************************/
	enum AudioList
	{
		BGM = 0,
		HIT,
		READY,
		CONFIRMCLICK,
	};

	/*!***********************************************************************
	\brief
	 List of all the Audio Channels
	*************************************************************************/
	enum ChannelGroups
	{
		ENGINEAUDIO = 0,
		MENUAUDIO,
		LEVEL1,
		LAST
	};
}
