#pragma once
#include"Hazel/Core.h"
#include"spdlog/spdlog.h"
#include"spdlog/fmt/ostr.h"

namespace Hazel {

class  HAZEL_API Log
{//static 表明我们不需要对实例也就是对象去做单独的处理，不需要实例就可以调用相应的函数和字段。
	//inline 主要是使得函数可以在多个地方被
//	定义（应用），绑定类型由全局转变为weak,链接的时候更不会出错
public:
	static void Init();
	inline static std::shared_ptr<spdlog::logger>& GetsCoreLogger() { return s_CoreLogger; }
	inline static std::shared_ptr<spdlog::logger>& GetsClientLogger(){ return s_ClientLogger; }

//不太清楚这里的静态应用的具体作用，这里面实在头文件中声明或定义（一定是要导出到dll里面的，
// 不然客户端文件找不到声明，因为客户端函数链接时要根据声明去找到定义存储的位置）
private:
	static std::shared_ptr<spdlog::logger>  s_CoreLogger;
	static std::shared_ptr<spdlog::logger>  s_ClientLogger;
	};
}
//利用宏定义的方式去更改使用库的接口
//core 状态下的日志宏
#define HZ_CORE_ERROR(...)	::Hazel::Log::GetsCoreLogger()->error(__VA_ARGS__)
#define HZ_CORE_WARN(...)	::Hazel::Log::GetsCoreLogger()->warn(__VA_ARGS__)
#define HZ_CORE_INFO(...)	::Hazel::Log::GetsCoreLogger()->info(__VA_ARGS__)
#define HZ_CORE_TRACE(...)	::Hazel::Log::GetsCoreLogger()->trace(__VA_ARGS__)
#define HZ_CORE_FATAL(...)	::Hazel::Log::GetsCoreLogger()->fatal(__VA_ARGS__)

//客户端状态下的日志宏
#define HZ_ERROR(...)		::Hazel::Log::GetsClientlogger()->error(__VA_ARGS__)
#define HZ_WARN(...)		::Hazel::Log::GetsClientLogger()->warn(__VA_ARGS__)
#define HZ_INFO(...)		::Hazel::Log::GetsClientLogger()->info(__VA_ARGS__)
#define HZ_TRACE(...)		::Hazel::Log::GetsClientLogger()->trace(__VA_ARGS__)
#define HZ_FATAL(...)		::Hazel::Log::GetsClientLogger()->fatal(__VA_ARGS__)