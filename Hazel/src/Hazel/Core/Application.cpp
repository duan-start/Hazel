#include "hzpch.h"

#include "Application.h"
#include "Hazel/Events/ApplicationEvent.h"
#include"Log.h"

#include "Hazel/Renderer/Renderer.h"
#include <GLFW/glfw3.h>

namespace Hazel {



	//这个的用法
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;


	Application::Application() {
		HZ_CORE_ASSERT(!s_Instance, "Application has been existed ");
		s_Instance = this;
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		//按理是要在析构函数中delete掉的，
		m_ImGuiLayer = new ImguiLayer();

		Renderer::Init();
		PushOverlay(m_ImGuiLayer);
		std::cout << "m_ImGuiLayer\n";
	}

	Application:: ~Application() {
		//不用写，这个实际上实在laystack手动删除，在那个地方管理生命周期
		//delete m_ImGuiLayer;  // 手动释放 m_ImGuiLayer
	}

	//设置事件回调函数
	void Application::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		
		//HZ_CORE_TRACE("{0}", e);
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
		//	HZ_CORE_TRACE("Layer order: {}", (*it)->GetName());
			(*it)->OnEvent(e);
		//	HZ_CORE_TRACE("event: {} ", e.Handled);
			if (e.Handled) break;  // 关键：立即终止
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

			//自定义的层栈的更新
			if (!m_Minimized) {
			for (Layer* layer : m_LayerStack) 
				layer->OnUpdate(timestep);
			}
			//ImGui的更新
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();
				//auto [mx, my] = Input::GetMousePosition();
				//HZ_CORE_TRACE("{0}, {1}", mx, my);


			//窗口画面的更新
			m_Window->OnUpdate();

		}
	}
}