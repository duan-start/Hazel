#include "hzpch.h"

#include "Log.h"
#include"spdlog/sinks/stdout_color_sinks.h"


namespace Hazel {
	 std::shared_ptr<spdlog::logger>  Log::s_ClientLogger;
	 std::shared_ptr<spdlog::logger>  Log::s_CoreLogger;
	//在实际的单元中定义，头(类)文件中仅仅只是声明（不分配内存的）
	void Log::Init() {
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_CoreLogger = spdlog::stdout_color_mt("Hazel");
		s_CoreLogger->set_level(spdlog::level::trace);

		s_ClientLogger = spdlog::stdout_color_mt("App");
		s_ClientLogger->set_level(spdlog::level::trace);
	}
}