#include "hzpch.h"

#include "Application.h"
#include "Hazel/Events/ApplicationEvent.h"
#include"Log.h"

#include "Hazel/Renderer/Renderer.h"
#include <GLFW/glfw3.h>

namespace Hazel {

	//�󶨶�Ӧ�ĺ���ָ�룬����x����һ��������ָ�룩��this����ʾ�滻�Ĳ���1
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
	
	//static ���������inline�Ļ���Ҫ���ⲿ����
	Application* Application::s_Instance = nullptr;


	Application::Application(const std::string& name,ApplicationCommandLineArgs args) {
		HZ_PROFILE_FUNCTION();

		HZ_CORE_ASSERT(!s_Instance, "Application has been existed ");
		//���ڹ��캯�����г�ʼ������ָ̬�룬����ָ��ָ��������󣨣�
		//static ��������Ȼ���������ڳ�ʼ���������ܹ������ڸı�ֵ
		s_Instance = this;
		//base���������ڣ������¼��ص�������imguilayer
		//ͬ���Ƕ�̬�Ĵ���
		m_Window = std::unique_ptr<Window>(Window::Create(WindowProps(name)));
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		//������Ҫ������������delete���ģ�
		m_ImGuiLayer = new ImGuiLayer();

		//��Ⱦ���ĳ�ʼ��
		Renderer::Init();
		PushOverlay(m_ImGuiLayer);
		std::cout << "m_ImGuiLayer\n";
	}

	Application:: ~Application() {
		HZ_PROFILE_FUNCTION();

		//����д�����ʵ����ʵ��laystack�ֶ�ɾ�������Ǹ��ط�������������
		//delete m_ImGuiLayer;  // �ֶ��ͷ� m_ImGuiLayer
	}

	//�����¼��ص�����
	void Application::OnEvent(Event& e) {
		HZ_PROFILE_FUNCTION();
		//�����Լ��������¼�
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		
		//HZ_CORE_TRACE("{0}", e);
		//��β�˵�ǰ�棬��һ�����¼����д���
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
		//	HZ_CORE_TRACE("Layer order: {}", (*it)->GetName());
			(*it)->OnEvent(e);
		//	HZ_CORE_TRACE("event: {} ", e.Handled);
			//�������ɹ���ֱ���˳��ͺ���
			if (e.Handled) break;  // �ؼ���������ֹ
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		HZ_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		HZ_PROFILE_FUNCTION();

		m_LayerStack.PushOverLayer(overlay);
		overlay->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		HZ_PROFILE_FUNCTION();

		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		HZ_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0) {
			m_Minimized = true;
			return false;
		}

		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		m_Minimized = false;
		return false;
	}
	void Application::Close() {
		m_Running = false;
	}
	//���ĸ����߼�
	void Application::Run() {

		HZ_PROFILE_FUNCTION();

		while (m_Running) {

			HZ_PROFILE_SCOPE("RunLoop");

			float time = glfwGetTime();
			//���time ��m_LastFrameTime��һ�����е��¼�
			//���ݲ�ֵ�õ���һ֡��ʱ��Ȼ��ȥtick����
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			//��ջ����
			if (!m_Minimized) {
			HZ_PROFILE_SCOPE("LayerStack Update");
			for (Layer* layer : m_LayerStack) 
				layer->OnUpdate(timestep);
			}

			//ImGui�ĸ���
			m_ImGuiLayer->Begin();
			{
			HZ_PROFILE_SCOPE("LayerStack OnImGuiRender");

			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();
			//auto [mx, my] = Input::GetMousePosition();
			//HZ_CORE_TRACE("{0}, {1}", mx, my);

			//���ڻ���ĸ���
			m_Window->OnUpdate();

		}
	}
}