#pragma once
//#ifdef HZ_PLATFORM_WINDOWS

//o ,对了，所有的头文件都开放给客户端，只是lib不开放

//这个头文件就是专门用来给客户端包含的，用来客户端的main的包含
//lib/exe，所有的.h不会主动的编译，

#include "Hazel/Core/Application.h"

//显示声明外部的函数，这个是一个多态的客户端的程序
extern Hazel::Application* Hazel::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc,char**argv) {
	//初始化log
	Hazel::Log::Init();
	
	HZ_PROFILE_BEGIN_SESSION("Start up", "HazelProfile-Startup.json");
	//这个鸡毛函数并不是类内直接定义的，而是一个完全未定义的函数，而且这个也不需要编译，所以也不会报错
	//这里创建的就是一个子类（客户端）
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

}//把入口点扔到这里面来了

//#endif