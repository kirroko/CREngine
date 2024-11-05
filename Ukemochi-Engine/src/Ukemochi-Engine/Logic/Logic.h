/* Start Header
*****************************************************************/
/*!
\file	Logic.h
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	22/10/24
\brief	This files implements the logic system that handles scripting of each entity within the game

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include "Scripting.h"
#include "../ECS/Systems.h"

namespace Ukemochi
{
	class LogicSystem : public System
	{
	public:
		LogicSystem();
		~LogicSystem() = default;
		LogicSystem(LogicSystem& other) = delete;
		LogicSystem& operator=(LogicSystem& other) = delete;
		LogicSystem& operator=(LogicSystem&& other) = delete;
		LogicSystem(LogicSystem&& other) = delete;
		
		/**
		 * @brief Init mono and Invoke all script Start Method
		 */
		void Init();

		/**
		 * @brief Invoke all entity's script Update Method
		 */
		void Update();

		/**
		 * @brief Remove all script instance
		 */
		void End();
	};
}