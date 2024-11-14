/* Start Header ************************************************************************/
/*!
\file       Log.h
\author     Hurng Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu
\date       Sept 14, 2024
\brief      This file defines the logging interface for the Ukemochi
			Engine, providing static methods for logging messages
			from both the engine and client(game) perspectives using
			the spdlog library.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include "../Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

#pragma warning(push)
#pragma warning(disable : 4251)  // Disable warning C4251

namespace Ukemochi {

	class UME_API Log
	{
	public:
		/*!
		\brief Initializes the logging system.
		*/
		static void Init();
		/*!
		\brief Gets the engine logger.
		\return A reference to the engine logger.
		*/
		inline static std::shared_ptr<spdlog::logger>& GetEngineLogger() { return s_EngineLogger; }

		/*!
		\brief Gets the client logger.
		\return A reference to the client logger.
		*/
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_EngineLogger; ///< Logger for engine messages
		static std::shared_ptr<spdlog::logger> s_ClientLogger; ///< Logger for client/game messages
	};
}

#pragma warning(pop)  // Re-enable the warning

// Logging levels:
// TRACE (Capture execution of code)
// INFO (Capture an event that occurred)
// WARN (Indicate unexpected event, disrupt or delay)
// ERROR (Capture a system interfering with functionalities)
// FATAL (Capture a system crash)
 // Engine log macros
 #define UME_ENGINE_TRACE(...) ::Ukemochi::Log::GetEngineLogger() -> trace(__VA_ARGS__)
 #define UME_ENGINE_INFO(...)  ::Ukemochi::Log::GetEngineLogger() -> info(__VA_ARGS__)
 #define UME_ENGINE_WARN(...)  ::Ukemochi::Log::GetEngineLogger() -> warn(__VA_ARGS__)
 #define UME_ENGINE_ERROR(...) ::Ukemochi::Log::GetEngineLogger() -> error(__VA_ARGS__)
 #define UME_ENGINE_FATAL(...) ::Ukemochi::Log::GetEngineLogger() -> critical(__VA_ARGS__)

 // Client log marcros
 #define UME_TRACE(...)        ::Ukemochi::Log::GetClientLogger() -> trace(__VA_ARGS__)
 #define UME_INFO(...)         ::Ukemochi::Log::GetClientLogger() -> info(__VA_ARGS__)
 #define UME_WARN(...)         ::Ukemochi::Log::GetClientLogger() -> warn(__VA_ARGS__)
 #define UME_ERROR(...)        ::Ukemochi::Log::GetClientLogger() -> error(__VA_ARGS__)
 #define UME_FATAL(...)        ::Ukemochi::Log::GetClientLogger() -> critical(__VA_ARGS__)
//#define UME_ENGINE_TRACE(...) ::UME::Log::GetEngineLogger() -> trace(__VA_ARGS__)
//#define UME_ENGINE_INFO(...)  ::UME::Log::GetEngineLogger() -> info(__VA_ARGS__)
//#define UME_ENGINE_WARN(...)  ::UME::Log::GetEngineLogger() -> warn(__VA_ARGS__)
//#define UME_ENGINE_ERROR(...) ::UME::Log::GetEngineLogger() -> error(__VA_ARGS__)
//#define UME_ENGINE_FATAL(...) ::UME::Log::GetEngineLogger() -> critical(__VA_ARGS__)
//
//// Client log marcros
//#define UME_TRACE(...)        ::UME::Log::GetClientLogger() -> trace(__VA_ARGS__)
//#define UME_INFO(...)         ::UME::Log::GetClientLogger() -> info(__VA_ARGS__)
//#define UME_WARN(...)         ::UME::Log::GetClientLogger() -> warn(__VA_ARGS__)
//#define UME_ERROR(...)        ::UME::Log::GetClientLogger() -> error(__VA_ARGS__)
//#define UME_FATAL(...)        ::UME::Log::GetClientLogger() -> critical(__VA_ARGS__)

// if dist build
//#define CR_ENGINE_INFO
