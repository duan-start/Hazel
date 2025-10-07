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
	//main函数里面的argi和count的传参
	//添加更多的注释，方便更好的理解
	struct ApplicationCommandLineArgs
	{
		//命令行直接打包内容
		int Count = 0;
		char** Args = nullptr;

		//数据的访问
		const char* operator[](int index) const
		{
			HZ_CORE_ASSERT(index < Count,"Index Out of Count");
			return Args[index];
		}
	};
	//Application是一个策略模式，客户端直接继承这个类，同时定义CreatApplicaton 函数
class HAZEL_API Application {
public:	
	Application(const std::string& name, ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
	virtual ~Application();
	//核心的循环逻辑
	void Run();
	//事件处理机制，里面创建一个dispatch,用来晚绑定的对应的函数
	//接受一个事件并进行处理
	void OnEvent(Event& e);
	//这个就是不同的页面，用来进行不同的事件处理和捕获
	//正常的push
	void PushLayer(Layer* layer);
	//直接push到最上面
	void PushOverlay(Layer* overlay);

	//直接close
	void Close();
	//为了获得对应私有变量的数据，这个是全局唯一的数据资源
	inline static Application& Get() { return *s_Instance; }
	//获得对应的窗口资源
	Window& GetWindow(){ return  *m_Window; }
	//活得对应的imguilayer
	ImGuiLayer* GetImGuiLayer() {	return m_ImGuiLayer;}
	//获得简单的参数，负责不同的任务指令
	ApplicationCommandLineArgs GetCommandLineArgs()const { return m_CommandLineArgs;}
private:
	//每个application或者layer都需要手动定义自己的事件处理函数
	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);
private:
	//全局的资源：
	//窗口，imguilayer,layerstack(用来存layers,)
	//这个窗口也是封装过后的
	std::unique_ptr<Window> m_Window;
	bool m_Running = true;
	//layer的容器
	LayerStack m_LayerStack;
	ImGuiLayer* m_ImGuiLayer;

	ApplicationCommandLineArgs m_CommandLineArgs;
	
	bool m_Minimized = false;
	//wait a minute
	float m_LastFrameTime = 0.f;
private:
	//类内唯一，
	static Application* s_Instance;
};
//在客户端将会被定义，如果在类内定义的话，就必须知道子类是什么，但是这里就不需要知道，延迟了
Application* CreateApplication(ApplicationCommandLineArgs args);
}
