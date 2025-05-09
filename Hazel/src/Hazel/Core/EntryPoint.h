#pragma once
#ifdef HZ_PLATFORM_WINDOWS


extern Hazel::Application* Hazel::CreateApplication();

int main(int argc,char**argv) {
	 Hazel::Log::Init();
	 std::cout << "Starting application..." << std::endl;
	HZ_CORE_WARN("Initialized Log!");
	auto app = Hazel::CreateApplication();
	app->Run();
	delete app;
}//����ڵ��ӵ�����������

#endif