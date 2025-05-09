#include "hzpch.h"

#include "Application.h"
#include "Hazel/Events/ApplicationEvent.h"
#include"Log.h"

#include "Hazel/Renderer/Renderer.h"
#include <GLFW/glfw3.h>

namespace Hazel {



	//������÷�
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;


	Application::Application() {
		HZ_CORE_ASSERT(!s_Instance, "Application has been existed ");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		//������Ҫ������������delete���ģ�
		m_ImGuiLayer = new ImguiLayer();

		Renderer::Init();
		PushOverlay(m_ImGuiLayer);
		std::cout << "m_ImGuiLayer\n";
	}

	Application:: ~Application() {
		//����д�����ʵ����ʵ��laystack�ֶ�ɾ�������Ǹ��ط�������������
		//delete m_ImGuiLayer;  // �ֶ��ͷ� m_ImGuiLayer
	}

	//�����¼��ص�����
	void Application::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		
		//HZ_CORE_TRACE("{0}", e);
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
		//	HZ_CORE_TRACE("Layer order: {}", (*it)->GetName());
			(*it)->OnEvent(e);
		//	HZ_CORE_TRACE("event: {} ", e.Handled);
			if (e.Handled) break;  // �ؼ���������ֹ
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverLayer(overlay);
		overlay->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0) {
			m_Minimized = true;
			return false;
		}

		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		m_Minimized = false;
		return false;
	}


	void Application::Run() {

		while (m_Running) {

			float time = glfwGetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			//�Զ���Ĳ�ջ�ĸ���
			if (!m_Minimized) {
			for (Layer* layer : m_LayerStack) 
				layer->OnUpdate(timestep);
			}
			//ImGui�ĸ���
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();
				//auto [mx, my] = Input::GetMousePosition();
				//HZ_CORE_TRACE("{0}, {1}", mx, my);


			//���ڻ���ĸ���
			m_Window->OnUpdate();

		}
	}
}