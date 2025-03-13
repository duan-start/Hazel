#pragma once
#include"Hazel/Core.h"
#include "Window.h"

#include"Events/Event.h"
#include"Events/ApplicationEvent.h"

#include "Hazel/Layer.h"
#include "Hazel/LayerStack.h"

#include "Hazel/Input.h"
#include "Hazel/Imgui/ImguiLayer.h"

#include "Hazel/Renderer/Buffer.h"
#include "Hazel/Renderer/Shader.h"


namespace Hazel {
class HAZEL_API Application {
public:	
	Application();
	virtual ~Application();
	void Run();
	void OnEvent(Event& e);
	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);
	inline static Application& Get() { return *s_Instance; }
	inline Window& GetWindow(){ return  *m_Window; }
private:
	bool OnWindowClose(WindowClose& e);
	std::unique_ptr<Window> m_Window;
	bool m_Running = true;
	LayerStack m_LayerStack;
	ImguiLayer* m_ImGuiLayer;

	//快速hack实现的三角形绘制
	unsigned int m_vertexarray;
	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<VerTexBuffer> m_VertexBuffer;
	std::unique_ptr<IndexBuffer> m_IndexBuffer;

private:
	static Application* s_Instance;
};
//在客户端将会被定义
Application* CreateApplication();
}
