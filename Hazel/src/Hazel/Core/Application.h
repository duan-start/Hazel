#pragma once
#include "Hazel/Core/Core.h"
#include "Window.h"

#include "Hazel/Core/Timestep.h"

#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"

#include "Hazel/Core/Layer.h"
#include "Hazel/Core/LayerStack.h"

#include "Hazel/Core/Input.h"
#include "Hazel/Imgui/ImguiLayer.h"

#include "Hazel/Renderer/Buffer.h"
#include "Hazel/Renderer/Shader.h"

#include "Hazel/Renderer/VertexArray.h"

#include "Hazel/Renderer/Camera.h"

namespace Hazel {
	//main���������argi��count�Ĵ���
	//��Ӹ����ע�ͣ�������õ����
	struct ApplicationCommandLineArgs
	{
		//������ֱ�Ӵ������
		int Count = 0;
		char** Args = nullptr;

		//���ݵķ���
		const char* operator[](int index) const
		{
			HZ_CORE_ASSERT(index < Count,"Index Out of Count");
			return Args[index];
		}
	};
	//Application��һ������ģʽ���ͻ���ֱ�Ӽ̳�����࣬ͬʱ����CreatApplicaton ����
class HAZEL_API Application {
public:	
	Application(const std::string& name, ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
	virtual ~Application();
	//���ĵ�ѭ���߼�
	void Run();
	//�¼�������ƣ����洴��һ��dispatch,������󶨵Ķ�Ӧ�ĺ���
	//����һ���¼������д���
	void OnEvent(Event& e);
	//������ǲ�ͬ��ҳ�棬�������в�ͬ���¼�����Ͳ���
	//������push
	void PushLayer(Layer* layer);
	//ֱ��push��������
	void PushOverlay(Layer* overlay);

	//ֱ��close
	void Close();
	//Ϊ�˻�ö�Ӧ˽�б��������ݣ������ȫ��Ψһ��������Դ
	inline static Application& Get() { return *s_Instance; }
	//��ö�Ӧ�Ĵ�����Դ
	Window& GetWindow(){ return  *m_Window; }
	//��ö�Ӧ��imguilayer
	ImGuiLayer* GetImGuiLayer() {	return m_ImGuiLayer;}
	//��ü򵥵Ĳ���������ͬ������ָ��
	ApplicationCommandLineArgs GetCommandLineArgs()const { return m_CommandLineArgs;}
private:
	//ÿ��application����layer����Ҫ�ֶ������Լ����¼�������
	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);
private:
	//ȫ�ֵ���Դ��
	//���ڣ�imguilayer,layerstack(������layers,)
	//�������Ҳ�Ƿ�װ�����
	std::unique_ptr<Window> m_Window;
	bool m_Running = true;
	//layer������
	LayerStack m_LayerStack;
	ImGuiLayer* m_ImGuiLayer;

	ApplicationCommandLineArgs m_CommandLineArgs;
	
	bool m_Minimized = false;
	//wait a minute
	float m_LastFrameTime = 0.f;
private:
	//����Ψһ��
	static Application* s_Instance;
};
//�ڿͻ��˽��ᱻ���壬��������ڶ���Ļ����ͱ���֪��������ʲô����������Ͳ���Ҫ֪�����ӳ���
Application* CreateApplication(ApplicationCommandLineArgs args);
}
