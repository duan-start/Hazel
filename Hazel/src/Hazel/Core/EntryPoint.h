#pragma once
//#ifdef HZ_PLATFORM_WINDOWS
#include "Hazel/Core/Application.h"

extern Hazel::Application* Hazel::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc,char**argv) {
	Hazel::Log::Init();
	
	HZ_PROFILE_BEGIN_SESSION("Start up", "HazelProfile-Startup.json");
	auto app = Hazel::CreateApplication({argc,argv});
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("Runtime", "HazelProfile-Runtime.json");
	app->Run();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("Shutdown", "HazelProfile-Shutdown.json");
	delete app;
	HZ_PROFILE_END_SESSION();

}//����ڵ��ӵ�����������

//#endif