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
class HAZEL_API Application {
public:	
	Application(const std::string& name);
	virtual ~Application();
	void Run();
	void OnEvent(Event& e);
	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);
	void Close();
	inline static Application& Get() { return *s_Instance; }
	inline Window& GetWindow(){ return  *m_Window; }
private:
	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);
private:
	std::unique_ptr<Window> m_Window;
	bool m_Running = true;
	LayerStack m_LayerStack;
	ImguiLayer* m_ImGuiLayer;

	bool m_Minimized = false;
	float m_LastFrameTime = 0.f;
private:
	static Application* s_Instance;
};
//在客户端将会被定义
Application* CreateApplication();
}
