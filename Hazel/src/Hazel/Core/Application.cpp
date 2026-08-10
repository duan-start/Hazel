#include "hzpch.h"

#include "Application.h"
#include "Hazel/Events/ApplicationEvent.h"
#include"Log.h"

#include "Hazel/Renderer/Renderer3D.h"
#include <GLFW/glfw3.h>

namespace Hazel {

	//函数指针签名更改，使用this是显示替换的参数1。x(this,a)->x(a);(对外部暴露的接口)
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
	
	//static 数据如果不inline的话需要在外部定义
	Application* Application::s_Instance = nullptr;


	Application::Application(const std::string& name,ApplicationCommandLineArgs args) {
		HZ_PROFILE_FUNCTION();

		HZ_CORE_ASSERT(!s_Instance, "Application has been existed ");
		//类内构造函数进行初始化，多态指针，父类指针指向子类对象（这个是给子类集成实现多态的）
		s_Instance = this;

		//Important
		//Init Window,设置data里面的回调函数，实现事件的传递处理
		m_Window = std::unique_ptr<Window>(Window::Create(WindowProps(name)));
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		//Init ImguiLayer(Imgui是一个状态机)
		//所有的ImguiRender都是基于第一个Imgui的状态
		m_ImGuiLayer = new ImGuiLayer();
		PushOverLayer(m_ImGuiLayer);

		//Init Render
		Renderer3D::Init();
		
		//ScriptEngine::Init();
	}

	Application:: ~Application() {
		HZ_PROFILE_FUNCTION();

		//不用写，这个实际上实在laystack手动删除，在那个地方管理生命周期
		//delete m_ImGuiLayer;  
		ScriptEngine::Shutdown();
	}

	//Application处理事件的手段，通过阻塞式的方法逐个处理（每个事件都去轮询1.不同事件类型2.不同层栈）
	void Application::OnEvent(Event& e) {
		HZ_PROFILE_FUNCTION();
		//设置处理事件的类（保存这个事件）
		EventDispatcher dispatcher(e);
		//设置处理对应事件的逻辑
		//为了保证函数指针的签名相同，这里用bind实现了函数的adapt，保证函数能够成功绑定到对应指针
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		
		//从尾端到前面，逐Layer 实现传递事件进行处理
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
			(*it)->OnEvent(e);
		//	HZ_CORE_TRACE("event: {} ", e.Handled);
			//如果处理成功，直接退出
			if (e.Handled) break;  
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		HZ_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverLayer(Layer* overlay)
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

		Renderer3D::OnWindowResize(e.GetWidth(), e.GetHeight());
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

			//用上一帧的时间去tick物理
			//会存在一点问题
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			//逻辑，动画，渲染的离屏更新
			if (!m_Minimized) {
			HZ_PROFILE_SCOPE("LayerStack Update");
			for (Layer* layer : m_LayerStack) 
				layer->OnUpdate(timestep);
			}

			//ImGui的更新，状态机（绘制到屏幕的状态更新，2DUI）
			m_ImGuiLayer->Begin();
			{
			HZ_PROFILE_SCOPE("LayerStack OnImGuiRender");

			for (Layer* layer : m_LayerStack) {
				layer->OnImGuiRender();
			}
			}
			m_ImGuiLayer->End();

			//事件捕获和窗口画面更新
			m_Window->OnUpdate();

		}
	}
}