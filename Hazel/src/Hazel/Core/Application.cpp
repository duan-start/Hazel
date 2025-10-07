#include "hzpch.h"

#include "Application.h"
#include "Hazel/Events/ApplicationEvent.h"
#include"Log.h"

#include "Hazel/Renderer/Renderer.h"
#include <GLFW/glfw3.h>

namespace Hazel {

	//绑定对应的函数指针，其中x是是一个函数（指针），this是显示替换的参数1
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
	
	//static 数据如果不inline的话需要在外部定义
	Application* Application::s_Instance = nullptr;


	Application::Application(const std::string& name,ApplicationCommandLineArgs args) {
		HZ_PROFILE_FUNCTION();

		HZ_CORE_ASSERT(!s_Instance, "Application has been existed ");
		//类内构造函数进行初始化，多态指针，父类指针指向子类对象（）
		//static 的数据虽然不能在类内初始化，但是能够在类内改变值
		s_Instance = this;
		//base，创建窗口，设置事件回调，创建imguilayer
		//同样是多态的窗口
		m_Window = std::unique_ptr<Window>(Window::Create(WindowProps(name)));
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		//按理是要在析构函数中delete掉的，
		m_ImGuiLayer = new ImGuiLayer();

		//渲染器的初始化
		Renderer::Init();
		PushOverlay(m_ImGuiLayer);
		std::cout << "m_ImGuiLayer\n";
	}

	Application:: ~Application() {
		HZ_PROFILE_FUNCTION();

		//不用写，这个实际上实在laystack手动删除，在那个地方管理生命周期
		//delete m_ImGuiLayer;  // 手动释放 m_ImGuiLayer
	}

	//设置事件回调函数
	void Application::OnEvent(Event& e) {
		HZ_PROFILE_FUNCTION();
		//优先自己处理函数事件
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		
		//HZ_CORE_TRACE("{0}", e);
		//从尾端到前面，逐一传递事件进行处理
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
		//	HZ_CORE_TRACE("Layer order: {}", (*it)->GetName());
			(*it)->OnEvent(e);
		//	HZ_CORE_TRACE("event: {} ", e.Handled);
			//如果处理成功，直接退出就好了
			if (e.Handled) break;  // 关键：立即终止
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
	//核心更新逻辑
	void Application::Run() {

		HZ_PROFILE_FUNCTION();

		while (m_Running) {

			HZ_PROFILE_SCOPE("RunLoop");

			float time = glfwGetTime();
			//这个time 和m_LastFrameTime是一个运行的事件
			//根据差值得到这一帧的时候，然后去tick物理
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			//层栈更新
			if (!m_Minimized) {
			HZ_PROFILE_SCOPE("LayerStack Update");
			for (Layer* layer : m_LayerStack) 
				layer->OnUpdate(timestep);
			}

			//ImGui的更新
			m_ImGuiLayer->Begin();
			{
			HZ_PROFILE_SCOPE("LayerStack OnImGuiRender");

			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();
			//auto [mx, my] = Input::GetMousePosition();
			//HZ_CORE_TRACE("{0}, {1}", mx, my);

			//窗口画面的更新
			m_Window->OnUpdate();

		}
	}
}