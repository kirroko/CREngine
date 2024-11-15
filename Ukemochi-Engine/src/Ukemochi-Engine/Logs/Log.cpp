/* Start Header ************************************************************************/
/*!
\file       Log.cpp
\author     Hurng Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu
\date       Sept 14, 2024
\brief      This file implements logging functionality for the Ukemochi 
            Engine using the spdlog library, providing an interface 
            for engine and client/game logging.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"

#include "../Logs/Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Ukemochi {
	// Static member initialization for engine and client loggers
	std::shared_ptr<spdlog::logger> Log::s_EngineLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	/*!
   \brief Initializes the logging system.

   Sets the log pattern for output and creates two loggers:
   one for the engine and one for the client, both with
   trace level logging enabled.
   */
	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$"); // color,time, engine/client, error msg 
												// Set the log pattern with color, time, logger name, and message
		s_EngineLogger = spdlog::stdout_color_mt("Cozy Raccon"); // Create engine logger
		s_EngineLogger->set_level(spdlog::level::trace); // Set level to trace
		s_ClientLogger = spdlog::stdout_color_mt("Game"); // Create client logger
		s_ClientLogger->set_level(spdlog::level::trace);  // Set level to trace
	}
}
