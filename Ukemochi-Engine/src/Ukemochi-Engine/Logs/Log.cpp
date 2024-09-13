#include "PreCompile.h"

#include "../Logs/Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace UME {

	std::shared_ptr<spdlog::logger> Log::s_EngineLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$"); // color,time, engine/client, error msg
		s_EngineLogger = spdlog::stdout_color_mt("Cozy Raccon");
		s_EngineLogger->set_level(spdlog::level::trace);
		s_ClientLogger = spdlog::stdout_color_mt("Game");
		s_ClientLogger->set_level(spdlog::level::trace);
	}
}
