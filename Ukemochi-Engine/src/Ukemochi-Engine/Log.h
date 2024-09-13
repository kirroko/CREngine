#pragma once
#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"


namespace UME {

	class UME_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetEngineLogger() { return s_EngineLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_EngineLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

// Engine log macros
#define UME_ENGINE_TRACE(...) ::UME::Log::GetEngineLogger() -> trace(__VA_ARGS__)
#define UME_ENGINE_INFO(...)  ::UME::Log::GetEngineLogger() -> info(__VA_ARGS__)
#define UME_ENGINE_WARN(...)  ::UME::Log::GetEngineLogger() -> warn(__VA_ARGS__)
#define UME_ENGINE_ERROR(...) ::UME::Log::GetEngineLogger() -> error(__VA_ARGS__)
#define UME_ENGINE_FATAL(...) ::UME::Log::GetEngineLogger() -> fatal(__VA_ARGS__)

// Client log marcros
#define UME_TRACE(...)        ::UME::Log::GetClientLogger() -> trace(__VA_ARGS__)
#define UME_INFO(...)         ::UME::Log::GetClientLogger() -> info(__VA_ARGS__)
#define UME_WARN(...)         ::UME::Log::GetClientLogger() -> warn(__VA_ARGS__)
#define UME_ERROR(...)        ::UME::Log::GetClientLogger() -> error(__VA_ARGS__)
#define UME_FATAL(...)        ::UME::Log::GetClientLogger() -> fatal(__VA_ARGS__)

// if dist build
//#define CR_ENGINE_INFO

