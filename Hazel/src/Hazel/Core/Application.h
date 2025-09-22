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
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			HZ_CORE_ASSERT(index < Count,"Index Out of Count");
			return Args[index];
		}
	};
class HAZEL_API Application {
public:	
	Application(const std::string& name, ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
	virtual ~Application();
	void Run();
	void OnEvent(Event& e);
	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);
	void Close();
	inline static Application& Get() { return *s_Instance; }
	Window& GetWindow(){ return  *m_Window; }
	ImGuiLayer* GetImGuiLayer() {	return m_ImGuiLayer;}
	ApplicationCommandLineArgs GetCommandLineArgs()const { return m_CommandLineArgs;}
private:
	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);
private:
	std::unique_ptr<Window> m_Window;
	bool m_Running = true;
	LayerStack m_LayerStack;
	ImGuiLayer* m_ImGuiLayer;

	ApplicationCommandLineArgs m_CommandLineArgs;
	bool m_Minimized = false;
	float m_LastFrameTime = 0.f;
private:
	static Application* s_Instance;
};
//在客户端将会被定义
Application* CreateApplication(ApplicationCommandLineArgs args);
}
