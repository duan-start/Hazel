#pragma once
//客户端使用引擎的方式：链接库，包含头文件（和我们使用其他库的方法是一样的）
//库文件不需要入口
//#ifdef HZ_PLATFORM_WINDOWS
#include "Hazel/Core/Application.h"

//显示声明外部的函数，这个是一个多态的客户端的程序，不声明也没关系，毕竟application里面声明了
//extern Hazel::Application* Hazel::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc,char**argv) {
	//初始化log
	Hazel::Log::Init();
	
	HZ_PROFILE_BEGIN_SESSION("Start up", "HazelProfile-Startup.json");
	//不编译就不需要具体实现
	auto app = Hazel::CreateApplication({argc,argv});
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("Runtime", "HazelProfile-Runtime.json");
	//application 核心运行函数
	app->Run();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("Shutdown", "HazelProfile-Shutdown.json");
	//指针的销毁
	delete app;
	HZ_PROFILE_END_SESSION();

}

//#endif