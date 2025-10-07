
#include "hzpch.h"
#include <Hazel.h>
#include "Hazel/Core/EntryPoint.h"

#include "EditorLayer.h"

class HazelEditor : public Hazel::Application {
public:
	//���๹�캯��ִ�е�ԭ��
	// �����ڹ��캯��֮ǰ��һ��Ҫ���ø���Ĺ��캯���ģ�ֻ���Ƿ���ʾ���ö���
	//���������default�������Ϳ��Բ��Ḹ�ࣨ���Զ����ã�������ͱ�����������ʾ����
	//Ȼ�����ִ����һ�鸸��Ĺ��캯����Ȼ��ִ����һ��PushLayer����
	HazelEditor(ApplicationCommandLineArgs args):Application("Hazel Editor", args)
	{
		PushLayer(new EditorLayer());

		//PushOverlay(new ExampleLayer());
		// GameTest
		//PushOverlay(new GameLayer());
	}
	~HazelEditor() {

	}
};
 
Hazel::Application* Hazel::CreateApplication(ApplicationCommandLineArgs args) {
	return new HazelEditor(args);
}