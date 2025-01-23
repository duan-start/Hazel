#pragma once
#include"Hazel/Core.h"
#include"spdlog/spdlog.h"
#include"spdlog/fmt/ostr.h"

namespace Hazel {

class  HAZEL_API Log
{//static �������ǲ���Ҫ��ʵ��Ҳ���Ƕ���ȥ�������Ĵ�������Ҫʵ���Ϳ��Ե�����Ӧ�ĺ������ֶΡ�
	//inline ��Ҫ��ʹ�ú��������ڶ���ط���
//	���壨Ӧ�ã�����������ȫ��ת��Ϊweak,���ӵ�ʱ����������
public:
	static void Init();
	inline static std::shared_ptr<spdlog::logger>& GetsCoreLogger() { return s_CoreLogger; }
	inline static std::shared_ptr<spdlog::logger>& GetsClientLogger(){ return s_ClientLogger; }

//��̫�������ľ�̬Ӧ�õľ������ã�������ʵ��ͷ�ļ����������壨һ����Ҫ������dll����ģ�
// ��Ȼ�ͻ����ļ��Ҳ�����������Ϊ�ͻ��˺�������ʱҪ��������ȥ�ҵ�����洢��λ�ã�
private:
	static std::shared_ptr<spdlog::logger>  s_CoreLogger;
	static std::shared_ptr<spdlog::logger>  s_ClientLogger;
	};
}
//���ú궨��ķ�ʽȥ����ʹ�ÿ�Ľӿ�
//core ״̬�µ���־��
#define HZ_CORE_ERROR(...)	::Hazel::Log::GetsCoreLogger()->error(__VA_ARGS__)
#define HZ_CORE_WARN(...)	::Hazel::Log::GetsCoreLogger()->warn(__VA_ARGS__)
#define HZ_CORE_INFO(...)	::Hazel::Log::GetsCoreLogger()->info(__VA_ARGS__)
#define HZ_CORE_TRACE(...)	::Hazel::Log::GetsCoreLogger()->trace(__VA_ARGS__)
#define HZ_CORE_FATAL(...)	::Hazel::Log::GetsCoreLogger()->fatal(__VA_ARGS__)

//�ͻ���״̬�µ���־��
#define HZ_ERROR(...)		::Hazel::Log::GetsClientlogger()->error(__VA_ARGS__)
#define HZ_WARN(...)		::Hazel::Log::GetsClientLogger()->warn(__VA_ARGS__)
#define HZ_INFO(...)		::Hazel::Log::GetsClientLogger()->info(__VA_ARGS__)
#define HZ_TRACE(...)		::Hazel::Log::GetsClientLogger()->trace(__VA_ARGS__)
#define HZ_FATAL(...)		::Hazel::Log::GetsClientLogger()->fatal(__VA_ARGS__)