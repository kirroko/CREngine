/* Start Header
*****************************************************************/
/*!
\file	DataSyncSystem.h
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	4/11/24
\brief	This file is responsible for syncing data between the C# data into C++ | Not is used!

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include "Ukemochi-Engine/ECS/Systems.h"

namespace Ukemochi
{
    class DataSyncSystem : public System
    {
    public:
        /**
         * @brief Handles the syncing of data between C# and C++
         */
        void SyncData();
    };
}
