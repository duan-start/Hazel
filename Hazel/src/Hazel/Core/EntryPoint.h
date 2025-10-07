#pragma once
//#ifdef HZ_PLATFORM_WINDOWS

//o ,���ˣ����е�ͷ�ļ������Ÿ��ͻ��ˣ�ֻ��lib������

//���ͷ�ļ�����ר���������ͻ��˰����ģ������ͻ��˵�main�İ���
//lib/exe�����е�.h���������ı��룬

#include "Hazel/Core/Application.h"

//��ʾ�����ⲿ�ĺ����������һ����̬�Ŀͻ��˵ĳ���
extern Hazel::Application* Hazel::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc,char**argv) {
	//��ʼ��log
	Hazel::Log::Init();
	
	HZ_PROFILE_BEGIN_SESSION("Start up", "HazelProfile-Startup.json");
	//�����ë��������������ֱ�Ӷ���ģ�����һ����ȫδ����ĺ������������Ҳ����Ҫ���룬����Ҳ���ᱨ��
	//���ﴴ���ľ���һ�����ࣨ�ͻ��ˣ�
	auto app = Hazel::CreateApplication({argc,argv});
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("Runtime", "HazelProfile-Runtime.json");
	//application �������к���
	app->Run();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("Shutdown", "HazelProfile-Shutdown.json");
	//ָ�������
	delete app;
	HZ_PROFILE_END_SESSION();

}//����ڵ��ӵ�����������

//#endif