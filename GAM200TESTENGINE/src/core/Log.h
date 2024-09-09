#pragma once
#include "core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"


namespace CR {

	class CR_API Log
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
#define CR_ENGINE_TRACE(...) ::CR::Log::GetEngineLogger() -> trace(__VA_ARGS__)
#define CR_ENGINE_INFO(...)  ::CR::Log::GetEngineLogger() -> info(__VA_ARGS__)
#define CR_ENGINE_WARN(...)  ::CR::Log::GetEngineLogger() -> warn(__VA_ARGS__)
#define CR_ENGINE_ERROR(...) ::CR::Log::GetEngineLogger() -> error(__VA_ARGS__)
#define CR_ENGINE_FATAL(...) ::CR::Log::GetEngineLogger() -> fatal(__VA_ARGS__)

// Client log marcros
#define CR_TRACE(...)        ::CR::Log::GetClientLogger() -> trace(__VA_ARGS__)
#define CR_INFO(...)         ::CR::Log::GetClientLogger() -> info(__VA_ARGS__)
#define CR_WARN(...)         ::CR::Log::GetClientLogger() -> warn(__VA_ARGS__)
#define CR_ERROR(...)        ::CR::Log::GetClientLogger() -> error(__VA_ARGS__)
#define CR_FATAL(...)        ::CR::Log::GetClientLogger() -> fatal(__VA_ARGS__)

// if dist build
//#define CR_ENGINE_INFO